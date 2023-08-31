#pragma once

#include <atomic>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <thread>
#include <utility>

namespace pnd {
namespace pando {
namespace storage_pool {

/** Allocates and deallocates individual fixed-size storage slots from a preallocated pool.
 * Allocation is wait free, and deallocation is thread safe. Thus, this class is suitable for
 * situations where a single low latency producer thread allocates storage that is deallocated in
 * multiple consumer threads.
 * @tparam slot_size The size (in bytes) of a single slot
 * @tparam slot_align The alignment (in bytes) of a single slot
 */
template <size_t slot_size, size_t slot_align>
class Pool {
 public:
  /** Preallocate pool_size slots. */
  Pool(size_t pool_size) : pool_size_{pool_size} {
    assert(on_deck_.is_lock_free());

    slots_ = std::make_unique<Slot[]>(pool_size_);

    // Initialize a singly linked list of slots
    for (size_t i = 0; i < pool_size_ - 1; ++i) {
      slots_[i].next = &slots_[i + 1];
    }

    // Use Slot 0 first
    on_deck_ = &slots_[0];
  }

  /** Allocate a slot from the pool.
   * Not thread safe.
   */
  void* Allocate() {
    Slot* ret = on_deck_.exchange(kOnDeckLocked, std::memory_order_acquire);

    if (ret) {
      on_deck_.store(ret->next, std::memory_order_relaxed);
      return ret;
    } else {
      // All slots are full
      on_deck_.store(ret, std::memory_order_relaxed);
      throw std::bad_alloc();
    }
  }

  /** Return a Slot to the pool.
   * Thread safe.
   */
  void Deallocate(void* ptr) {
    auto as_slot = reinterpret_cast<Slot*>(ptr);

    as_slot->next = on_deck_.load(std::memory_order_relaxed);

    do {
      // If on_deck_ is locked, spin until it's not
      while (as_slot->next == kOnDeckLocked) {
        std::this_thread::yield();
        as_slot->next = on_deck_.load(std::memory_order_relaxed);
      }
    } while (!on_deck_.compare_exchange_weak(
        as_slot->next, as_slot, std::memory_order_release, std::memory_order_relaxed));
  }

 private:
  struct Slot {
    alignas(slot_align) uint8_t storage[slot_size]; /**< Storage */
    Slot* next; /**< Slot to use after this one (or nullptr) */
  };

  size_t pool_size_; /**< The number of slots in slots_ */
  std::unique_ptr<Slot[]> slots_; /**< Pre-allocated storage */
  std::atomic<Slot*> on_deck_; /**< Slot to use for next allocation */

  // magic pointer value indicating that on_deck_ is locked by Allocate()
  Slot* const kOnDeckLocked = reinterpret_cast<Slot*>(1);
};

/** Helper template to specialize Pool for storing a particular type. */
template <class T>
using PoolFor = Pool<sizeof(T), alignof(T)>;

/** Custom deleter for objects allocated in a Pool. */
template <class T>
struct Deleter {
  void operator()(T* ptr) {
    ptr->~T();
    if (pool_) {
      pool_->Deallocate(ptr);
    }
  }

  Deleter() = default; // Required to make unique_ptr<T, Deleter<T>> default constructable
  explicit Deleter(const std::shared_ptr<PoolFor<T>>& pool) : pool_{pool} {}

 private:
  std::shared_ptr<PoolFor<T>> pool_;
};

/** Type returned by Factory<T>::Create. */
template <class T>
using FactoryHandle = std::unique_ptr<T, Deleter<T>>;

/** Factory class that constructs objects in storage allocated from a Pool.
 * Created objects are wrapped in a unique_ptr with a stored deleter that will
 * eventually return the associated storage to the pool.
 * @tparam T The type of the object to create/store
 */
template <class T>
class Factory {
 public:
  Factory(size_t pool_size) : pool_{std::make_shared<PoolFor<T>>(pool_size)} {}

  /** Construct an object in a slot from the pool.
   * @param args Arguments to forward to the object's constructor
   * @return a unique_ptr which owns the object, with a stored deleter that will
   * eventually return the associated storage to the pool
   */
  template <class... Args>
  FactoryHandle<T> Create(Args&&... args) {
    // Allocate
    void* storage = pool_->Allocate();

    // Construct
    T* t = new (storage) T(std::forward<Args>(args)...);

    // Wrap in std::unique_ptr with custom deleter
    return FactoryHandle<T>{t, Deleter<T>{pool_}};
  }

  /** Construct an object in a slot from the pool using default initialization.
   * For POD types, default initialization means zero initialization is not performed.
   * @return a unique_ptr which owns the object, with a stored deleter that will
   * eventually return the associated storage to the pool
   */
  FactoryHandle<T> CreateForOverwrite() {
    // Allocate
    void* storage = pool_->Allocate();

    // Construct
    T* t = new (storage) T;

    // Wrap in std::unique_ptr with custom deleter
    return FactoryHandle<T>{t, Deleter<T>{pool_}};
  }

 private:
  std::shared_ptr<PoolFor<T>> pool_;
};

} // namespace storage_pool
} // namespace pando
} // namespace pnd
