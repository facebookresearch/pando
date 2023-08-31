#pragma once

#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "reporter.h"
#include "types.h"

namespace pnd {
namespace pando {

/** Class representing a histogram of macro times.
 * @tparam BinDuration specialization of std::duration to be used for the demarcation of bin edges
 */
template <class BinDuration>
class Histogram : private std::vector<uint32_t> {
 public:
  using std::vector<uint32_t>::operator[];
  using std::vector<uint32_t>::cbegin;
  using std::vector<uint32_t>::begin;
  using std::vector<uint32_t>::end;
  using std::vector<uint32_t>::cend;
  using std::vector<uint32_t>::data;
  using std::vector<uint32_t>::size;

  /** @param bin_size bin width */
  Histogram(BinDuration bin_size) : bin_size_{bin_size} {};

  /** Create a histogram from a vector of macro times. */
  void BinMacroTimes(const MacroTimes& macro_times, size_t begin_bin_idx, size_t end_bin_idx) {
    SetSpan(begin_bin_idx, end_bin_idx);

    // Handle empty input
    if (macro_times.size() == 0) {
      return;
    }

    if (macro_times.at(0) < (begin_bin_idx_ * bin_size_)) {
      throw std::out_of_range("Histogram::BinMacroTimes: first timestamp lies before first bin");
    }

    auto current_bin = this->begin();
    auto next_bin_edge = (begin_bin_idx_ + 1) * bin_size_;

    for (const auto& time : macro_times) {
      while (time >= next_bin_edge) {
        next_bin_edge += bin_size_;
        if (++current_bin >= this->end()) {
          throw std::out_of_range("Histogram::BinMacroTimes too few bins");
        }
      }

      // Increment count contained in current_bin
      ++(*current_bin);
    }
  }

  /** Create a histogram from an existing one by rebinning by an integer factor. */
  void Rebin(const Histogram<BinDuration>& source) {
    auto rebin_factor = bin_size_ / source.bin_size_;
    auto rebin_factor_rem = bin_size_ % source.bin_size_;
    if (rebin_factor_rem != rebin_factor_rem.zero()) {
      throw std::invalid_argument(
          "Histogram::Rebin: source.bin_size_ not an integer multiple of this.bin_size_");
    }

    if (source.size() % rebin_factor) {
      throw std::invalid_argument(
          "Histogram::Rebin: source size must be evenly divisible by rebin_factor");
    }

    if (&source != this) {
      this->resize(source.size() / rebin_factor);
    }

    auto src = source.cbegin();
    auto dst = this->begin();

    while (src < source.end()) {
      *dst = *src++;
      for (int i = 1; i < rebin_factor; ++i) {
        *dst += *src++;
      }
      ++dst;
    }
    this->resize(source.size() / rebin_factor);

    this->begin_bin_idx_ = source.begin_bin_idx_ / rebin_factor;
    this->end_bin_idx_ = source.end_bin_idx_ / rebin_factor;
  }

  BinDuration GetBinSize() const {
    return bin_size_;
  }

  size_t GetBeginBinIdx() const {
    return begin_bin_idx_;
  }

  size_t GetEndBinIdx() const {
    return end_bin_idx_;
  }

  void SetSpan(size_t begin_bin_idx, size_t end_bin_idx) {
    begin_bin_idx_ = begin_bin_idx;
    end_bin_idx_ = end_bin_idx;

    auto bin_count = end_bin_idx_ - begin_bin_idx_;
    this->assign(bin_count, 0);
  }

 private:
  BinDuration bin_size_;
  size_t begin_bin_idx_ = 0;
  size_t end_bin_idx_ = 0;
};

} // namespace pando
} // namespace pnd
