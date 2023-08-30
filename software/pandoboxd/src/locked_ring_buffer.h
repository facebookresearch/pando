#pragma once

#include <array>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <type_traits>

namespace pnd {
namespace pandoboxd {

/** Implements a zero-copy locked single producer single consumer ring buffer.
 *
 * Each item in the buffer is held inside a unique_ptr. If T is an array type, then the array
 * version of unique_ptr is used and the size of the arrays must be specified in the constructor.
 *
 * @tparam T The type of a single item in the buffer
 * @tparam N The number of slots in the buffer
 */
template <class T, size_t N>
class LockedRingBuffer {
  // If there's only 2 slots, write can't advance after the first read.
  static_assert(N > 2, "RingBuffer will deadlock");

 public:
  // If Item is an array type, ItemElement is the type of a single element of that array type.
  // Otherwise Item and ItemElement are the same type.
  using Item = T;
  using ItemElement = typename std::remove_extent_t<Item>;

  /** Constructor for non-array Item types. */
  LockedRingBuffer() {
    Reset();
    for (auto& buffer : buffers_) {
      buffer = std::make_unique<Item>();
    }
  }

  /** Constructor for array Item types.
   *@param size The number of elements in each array
   */
  template <
      class U = Item,
      class = std::enable_if_t<std::is_array<U>::value && std::is_same<U, Item>::value>>
  LockedRingBuffer(size_t size) {
    Reset();
    for (auto& buffer : buffers_) {
      buffer = std::make_unique<Item>(size);
    }
  }

  /** Commit the last Item returned (if any) and return an Item that is ready to be written to.
   *
   * If no Item is ready to be written to (overrun), the last Item returned is not comitted and
   * nullptr is returned.
   *
   * @return Pointer to an ItemElement, which may be the first element in an array type Item, or
   * nullptr
   */
  ItemElement* AdvanceWrite(int timeout) {
    std::unique_lock<std::mutex> mlock(mutex_);
    size_t prev_write = write_;
    size_t write = Next(write_);

    // Check for write catching read
    while (write == read_ || (prev_write == (kNumElements - 1) && read_ == SIZE_MAX)) {
      if (timeout >= 0) {
        if (cond_.wait_for(mlock, std::chrono::milliseconds{timeout}) == std::cv_status::timeout) {
          return nullptr;
        }
      } else {
        cond_.wait(mlock);
      }
    }

    write_ = write;

    cond_.notify_one();
    return buffers_[write].get();
  }

  /** Release the last Item returned (if any) and return an Item that is ready to be read from.
   *
   * If no Item is ready to be read (underflow), the last Item returned is not released and nullptr
   * is returned.
   *
   * @return Pointer to an ItemElement, which may be the first element in an array type Item, or
   * nullptr
   */
  const ItemElement* AdvanceRead(std::chrono::milliseconds::rep timeout) {
    std::unique_lock<std::mutex> mlock(mutex_);
    size_t read = Next(read_);

    // Check for read catching write
    while (read == write_ || write_ == SIZE_MAX) {
      if (timeout >= 0) {
        if (cond_.wait_for(mlock, std::chrono::milliseconds{timeout}) == std::cv_status::timeout) {
          return nullptr;
        }
      } else {
        cond_.wait(mlock);
      }
    }

    read_ = read;

    cond_.notify_one();
    return buffers_[read].get();
  }

  /** Reset write and read indices to reflect an empty state.
   * Does not destroy the contained elements.
   */
  void Reset() {
    std::unique_lock<std::mutex> mlock(mutex_);
    write_ = SIZE_MAX;
    read_ = SIZE_MAX;
  }

 private:
  static constexpr size_t kNumElements = N;
  static constexpr size_t kPadSize = 64 - sizeof(size_t);

  static size_t Next(size_t i) {
    return (i + 1) % kNumElements;
  }

  size_t write_;
  char pad1_[kPadSize];
  size_t read_;
  char pad2_[kPadSize];
  std::array<std::unique_ptr<Item>, kNumElements> buffers_;

  std::mutex mutex_;
  std::condition_variable cond_;
};

} // namespace pandoboxd
} // namespace pnd
