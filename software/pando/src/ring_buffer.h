#pragma once

#include <array>
#include <atomic>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace pnd {
namespace pando {

/** Implements a lock free single producer single consumer ring buffer.
 *
 * Each item in the buffer is held inside a unique_ptr. If T is an array type, then the array
 * version of unique_ptr is used and the size of the arrays must be specified in the constructor.
 *
 * @tparam T The type of a single item in the buffer
 * @tparam N The number of slots in the buffer
 */
template <class T, size_t N>
class RingBuffer {
  // If there's only 2 slots, write can't advance after the first read.
  static_assert(N > 2, "RingBuffer will deadlock");

 public:
  // If Item is an array type, ItemElement is the type of a single element of that array type.
  // Otherwise Item and ItemElement are the same type.
  using Item = T;
  using ItemElement = typename std::remove_extent_t<Item>;

  /** Constructor for non-array Item types. */
  RingBuffer() {
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
  RingBuffer(size_t size) {
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
  ItemElement* MaybeAdvanceWrite() {
    size_t prev_write = write_.load(std::memory_order_relaxed);
    size_t write = Next(prev_write);
    size_t read = read_.load(std::memory_order_acquire);

    // Check for write catching read
    if (write == read || (prev_write == (kNumElements - 1) && read == SIZE_MAX)) {
      // Overflow
      return nullptr;
    }

    write_.store(write, std::memory_order_release);

    return buffers_[write].get();
  }

  /** Commit the last Item returned (if any) and return an Item that is ready to be written to.
   * @return Pointer to an ItemElement, which may be the first element in an array type Item
   */
  ItemElement* AdvanceWrite() {
    auto ret = MaybeAdvanceWrite();
    if (!ret) {
      throw std::overflow_error("RingBuffer: Overrun");
    }
    return ret;
  }

  /** Release the last Item returned (if any) and return an Item that is ready to be read from.
   *
   * If no Item is ready to be read (underflow), the last Item returned is not released and nullptr
   * is returned.
   *
   * @return Pointer to an ItemElement, which may be the first element in an array type Item, or
   * nullptr
   */
  const ItemElement* AdvanceRead() {
    size_t write = write_.load(std::memory_order_acquire);
    size_t read = Next(read_.load(std::memory_order_relaxed));

    // Check for read catching write
    if (read == write || write == SIZE_MAX) {
      // Underflow
      return nullptr;
    }

    read_.store(read, std::memory_order_release);

    return buffers_[read].get();
  }

  /** Reset write and read indices to reflect an empty state.
   * Does not destroy the contained elements.
   */
  void Reset() {
    write_.store(SIZE_MAX, std::memory_order_relaxed);
    read_.store(SIZE_MAX, std::memory_order_release);
  }

 private:
  static constexpr size_t kNumElements = N;
  static constexpr size_t kPadSize = 64 - sizeof(size_t);

  static size_t Next(size_t i) {
    return (i + 1) % kNumElements;
  }

  std::atomic<size_t> write_;
  char pad1_[kPadSize];
  std::atomic<size_t> read_;
  char pad2_[kPadSize];
  std::array<std::unique_ptr<Item>, kNumElements> buffers_;
};

} // namespace pando
} // namespace pnd
