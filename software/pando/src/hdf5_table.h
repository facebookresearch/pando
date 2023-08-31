#pragma once

#include "mutex.h"
#include "reporter.h"

#include <H5Cpp.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <stdexcept>
#include <vector>

namespace pnd {
namespace pando {

class HDF5Table {
 public:
  struct HDF5TableException : std::runtime_error {
    using std::runtime_error::runtime_error;
  };

  using Buffer = std::vector<char>;

  static constexpr char kChannelsAttribute[] = "selected_channels";

  /** Constructor for creating a new dataset.
   * @param file File to create the dataset in
   * @param dtype Datatype of each element in the table
   * @param name Name of the dataset
   * @param channels Selected channels for dataset. Empty vector means don't use the attribute
   * @param chunk_size HDF5 chunk size (# of rows)
   */
  HDF5Table(
      H5::H5File& file,
      const H5::DataType& dtype,
      const std::string& name,
      const std::vector<int>& channels,
      hsize_t chunk_size)
      : file_(file),
        dtype_{dtype},
        dtype_size_(dtype_.getSize()),
        dims_{0, (channels.size() > 0) ? channels.size() : 1},
        n_dims_((dims_[1] > 1) ? 2 : 1),
        chunk_size_{chunk_size},
        chunk_nbytes_{row_nbytes_ * chunk_size_},
        channels_(channels) {
    H5LockGuard l;

    // Create the initial data space
    Dimensions max_dims{H5S_UNLIMITED, dims_[1]};
    H5::DataSpace dataspace{n_dims_, dims_.data(), max_dims.data()};

    // Bound chunk_size to ensure each chunk is less than 4GB
    auto max_chunk_size = kMaxChunkBytes / (dtype_size_ * dims_[1]);
    if (chunk_size > max_chunk_size) {
      chunk_size = max_chunk_size;
      g_reporter->warn(
          "HDF5Table: Limiting chunk size to {} rows for dset {} in order to stay below 4GB (dtype_size_={}, dims_[1] = {}).",
          chunk_size,
          name,
          dtype_size_,
          dims_[1]);
    }

    // Can't have a chunk size of 0 rows
    if (chunk_size < 1) {
      throw HDF5TableException("A single row is larger than the maximum allowable chunk size.");
    }

    // Configure chunking
    H5::DSetCreatPropList dset_params;
    hsize_t chunk_dims[2] = {chunk_size, dims_[1]};
    dset_params.setChunk(n_dims_, chunk_dims);

    // Create the data set
    dset_ = file_.createDataSet(name, dtype_, dataspace, dset_params);

    // attach channel numbers attribute, if needed
    if (channels.size() > 0) {
      const hsize_t dims = dims_[1];
      auto aspace = H5::DataSpace(1, &dims);
      auto channels_attr =
          dset_.createAttribute(kChannelsAttribute, H5::PredType::NATIVE_INT, aspace);
      channels_attr.write(H5::PredType::NATIVE_INT, reinterpret_cast<const void*>(channels.data()));
    }
  }

  /** Constructor for creating a new dataset. This version is a convenience to clients not using the
   * selected channels attribute
   * @param file File to create the dataset in
   * @param dtype Datatype of each element in the table
   * @param name Name of the dataset
   * @param chunk_size HDF5 chunk size (# of rows)
   */
  HDF5Table(
      H5::H5File& file,
      const H5::DataType& dtype,
      const std::string& name,
      hsize_t chunk_size)
      : HDF5Table(file, dtype, name, std::vector<int>(), chunk_size) {}

  /** Constructor for opening an existing dataset.
   * @param file File where the dataset is located
   * @param name Name of the dataset
   */
  HDF5Table(H5::H5File& file, const std::string& name)
      : file_(file),
        dset_(file_.openDataSet(name)),
        dtype_(dset_.getDataType()),
        dtype_size_(dtype_.getSize()) {
    H5LockGuard l;

    H5::DataSpace file_space = dset_.getSpace();
    n_dims_ = file_space.getSimpleExtentNdims();

    if (n_dims_ > std::tuple_size<Dimensions>::value) {
      throw HDF5TableException("Dataset has too many dimensions");
    }

    dims_[1] = 1; // 1 is overwritten if n_dims > 1
    file_space.getSimpleExtentDims(dims_.data());

    // Get the chunk size (# of rows)
    hsize_t chunk_dims[2];
    H5::DSetCreatPropList dset_params = dset_.getCreatePlist();
    dset_params.getChunk(n_dims_, chunk_dims);
    if (n_dims_ == 2 && chunk_dims[1] != dims_[1]) {
      HDF5TableException("Chunk width does not match dset width");
    }
    chunk_size_ = chunk_dims[0];
    chunk_nbytes_ = row_nbytes_ * chunk_size_;

    // read the channels attribute info
    if (dset_.attrExists(kChannelsAttribute)) {
      auto channels_attr = dset_.openAttribute(kChannelsAttribute);
      auto type = channels_attr.getDataType();
      channels_.assign(dims_[1], 0);
      channels_attr.read(type, channels_.data());
    } else {
      // no channels attribute
      // assume default channel scheme
      // (column number = channel number)
      for (int i = 0; i < dims_[1]; i++) {
        channels_.push_back(i);
      }
    }
  }

  ~HDF5Table() {
    WriteChunkBuff();
  }

  const std::vector<int>& GetChannels() const {
    return channels_;
  }

  const H5::DataType& GetDType() const {
    return dtype_;
  }

  hsize_t GetDTypeSize() const {
    return dtype_size_;
  }

  hsize_t GetNRows() const {
    return dims_[0];
  }

  hsize_t GetNCols() const {
    return dims_[1];
  }

  void Read(Buffer& buf, hsize_t row_offset, hsize_t n_rows) {
    H5LockGuard l;

    Dimensions size{n_rows, dims_[1]};
    Dimensions offset{row_offset, 0};

    H5::DataSpace file_space = dset_.getSpace();
    H5::DataSpace mem_space = H5::DataSpace(n_dims_, size.data());

    // Select hyperslab
    file_space.selectHyperslab(H5S_SELECT_SET, size.data(), offset.data());
    assert(file_space.selectValid());

    // Perform read
    buf.resize(dtype_size_ * mem_space.getSimpleExtentNpoints());
    dset_.read(buf.data(), dtype_, mem_space, file_space);
  }

  void Append(const Buffer& buf) {
    Append(buf.data(), buf.size());
  }

  void Append(const void* data, Buffer::size_type size) {
    assert(size % row_nbytes_ == 0);

    const hsize_t n_rows = size / row_nbytes_;
    for (hsize_t row_idx = 0; row_idx < n_rows;) {
      if (chunk_w_buff_occupancy_ == 0 && (n_rows - row_idx) >= chunk_size_) {
        // If chunk_w_buff_ is empty and we have at least a whole chunk left to write, bypass
        // chunk_w_buff_
        auto chunk_size_tmp = chunk_size_; // AppendChunk modifies its second arg
        AppendChunk(static_cast<const char*>(data) + row_idx * row_nbytes_, chunk_size_tmp);
        assert(chunk_size_tmp == 0);
        row_idx += chunk_size_;
      } else {
        // Else, copy data into chunk_w_buff_
        auto rows_to_copy = std::min(chunk_size_ - chunk_w_buff_occupancy_, n_rows - row_idx);

        // Allocate chunk_w_buff_ the first time we use it
        if (chunk_w_buff_.size() == 0) {
          chunk_w_buff_.assign(chunk_nbytes_, 0);
        }

        auto src_begin = static_cast<const char*>(data) + row_idx * row_nbytes_;
        auto src_end = src_begin + rows_to_copy * row_nbytes_;
        auto dst_begin = chunk_w_buff_.data() + chunk_w_buff_occupancy_ * row_nbytes_;

        std::copy(src_begin, src_end, dst_begin);
        chunk_w_buff_occupancy_ += rows_to_copy;

        // Flush chunk_w_buff_ when full
        if (chunk_w_buff_occupancy_ == chunk_size_) {
          AppendChunk(chunk_w_buff_.data(), chunk_w_buff_occupancy_);
        }

        row_idx += rows_to_copy;
      }
    }
  }

  /** Write out the current contents of chunk_w_buff_.
   * If chunk_w_buff_ is not full, its contents will be written out as a partial chunk and
   * chunk_w_buff_occupancy_ will remain unchanged so that the data is re-written in the next call.
   */
  void WriteChunkBuff() {
    if (chunk_w_buff_occupancy_) {
      std::fill(
          chunk_w_buff_.begin() + chunk_w_buff_occupancy_ * row_nbytes_, chunk_w_buff_.end(), 0);
      AppendChunk(chunk_w_buff_.data(), chunk_w_buff_occupancy_);
    }
  }

 private:
  using Dimensions = std::array<hsize_t, 2>;

  static constexpr size_t kMaxChunkBytes = 0xffffffff; // HDF5 limits chunks to 4GB

  /** Write the contents of a buffer to a chunk at the end of the dataset.
   * If n_rows == chunk_size_, n_rows is set to 0 to indicate that a complete chunk was written. If
   * n_rows < chunk_size_, nrows retains its starting value to indicate that a partial chunk was
   * written and any subsequent call to AppendChunk should begin with the same rows.
   *
   * @param buf a buffer of size chunk_nbytes_
   * @param n_rows the number of rows contained in buf
   */
  void AppendChunk(const void* buf, hsize_t& n_rows) {
    assert(n_rows <= chunk_size_);

    H5LockGuard l;

    // We will overwrite any partially written chunk
    dims_[0] -= dims_[0] % chunk_size_;

    // Extend the dataset to hold new rows
    auto new_dims = dims_;
    new_dims[0] += n_rows;
    dset_.extend(new_dims.data());

    Dimensions offset{dims_[0], 0};
    WriteChunk(offset.data(), chunk_nbytes_, buf);

    dims_ = new_dims;

    // Update n_rows argument with the number of just-written rows that will be overwritten in the
    // next call to AppendChunk (ie. 0 if we wrote a complete chunk ).
    n_rows = dims_[0] % chunk_size_;

    // Check that new dimensions are as expected
    assert([&] {
      Dimensions readback;
      readback[1] = 1;
      dset_.getSpace().getSimpleExtentDims(readback.data());
      return readback;
    }() == dims_);
  }

  /** Use the HDF5 C API to directly write a chunk to the dataset.
   * See: https://portal.hdfgroup.org/display/HDF5/Using+the+Direct+Chunk+Write+Function
   */
  void WriteChunk(const hsize_t* offset, size_t data_size, const void* buf) {
    herr_t ret_value = H5Dwrite_chunk(dset_.getId(), H5P_DEFAULT, 0x00, offset, data_size, buf);
    if (ret_value < 0) {
      throw HDF5TableException("H5Dwrite_chunk failed");
    }
  }

  const H5::H5File& file_;
  H5LockAdapter<H5::DataSet> dset_;
  H5LockAdapter<H5::DataType> dtype_;
  hsize_t dtype_size_;
  Dimensions dims_;
  int n_dims_;
  hsize_t chunk_size_; /**< The number of rows in a single chunk */
  hsize_t row_nbytes_{dtype_size_ * dims_[1]};
  hsize_t chunk_nbytes_;
  Buffer chunk_w_buff_;
  hsize_t chunk_w_buff_occupancy_ = 0; /**< The number of rows currently stored in chunk_w_buff_. */
  std::vector<int> channels_;
};

} // namespace pando
} // namespace pnd
