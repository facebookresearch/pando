#include "storage_pool.h"

#include "gtest/gtest.h"

#include <array>
#include <cstdint>
#include <memory>

namespace pnd {
namespace pando {
namespace storage_pool {

/** Initialize a Pool */
TEST(PoolTest, InitPool) {
  PoolFor<char> pool(1);
}

/** Allocate and deallocate storage from a Pool in nontrivial order */
TEST(PoolTest, Complicated) {
  // Create a pool with three slots, then use them all.
  PoolFor<char> pool(3);
  void* const c0 = pool.Allocate();
  void* const c1 = pool.Allocate();
  void* const c2 = pool.Allocate();

  // All slots are full
  EXPECT_THROW(pool.Allocate(), std::bad_alloc);

  // Make slot 2 free, then use it
  pool.Deallocate(c2);
  ASSERT_EQ(pool.Allocate(), c2);

  // All slots are full
  EXPECT_THROW(pool.Allocate(), std::bad_alloc);

  // Make slot 0 free, then use it
  pool.Deallocate(c0);
  ASSERT_EQ(pool.Allocate(), c0);

  // All slots are full
  EXPECT_THROW(pool.Allocate(), std::bad_alloc);

  // Clear slots 1 and 2, then use them
  pool.Deallocate(c1);
  pool.Deallocate(c2);
  ASSERT_EQ(pool.Allocate(), c2); // Slot 2 freed last, so used first
  ASSERT_EQ(pool.Allocate(), c1);

  // All slots are full
  EXPECT_THROW(pool.Allocate(), std::bad_alloc);
}

/** Create a pool whose slots are aligned to 64 bytes */
TEST(PoolTest, ExtendedAlignment) {
  static constexpr size_t kSlotSize = 1;
  static constexpr size_t kSlotAlign = 64;

  Pool<kSlotSize, kSlotAlign> p(2);

  // Get the addresses of slots 0 and 1
  void* const s0 = p.Allocate();
  void* const s1 = p.Allocate();

  // Check that both slots are aligned using std::align (std::align should make no changes)
  void* s0_ = s0;
  size_t space = kSlotSize;
  EXPECT_EQ(std::align(kSlotAlign, kSlotSize, s0_, space), s0);
  EXPECT_EQ(s0, s0_);
  EXPECT_EQ(space, kSlotSize);

  void* s1_ = s1;
  EXPECT_EQ(std::align(kSlotAlign, kSlotSize, s1_, space), s1);
  EXPECT_EQ(s1, s1_);
  EXPECT_EQ(space, kSlotSize);
}

/** Initialize a Factory */
TEST(FactoryTest, InitFactory) {
  Factory<char> factory(1);
}

/** Use a Factory to construct an instance of a class with no default constructor */
TEST(FactoryTest, Create) {
  struct Foo {
    Foo(int){};
  };

  Factory<Foo> factory(1);
  factory.Create(0);
}

/** Prove that using CreateForOverwrite results in default initialization. */
TEST(StoragePoolFactoryTest, CreateForOverwrite) {
  Factory<std::array<uint8_t, 4>> f(1);

  // Create an array in the pool, store a pointer to its data, then destruct it (but keep the
  // pointer)
  auto int1 = f.Create();
  volatile uint8_t* vp = int1->data();
  int1.reset();

  // vp now points to the slot in the pool that will be used for the next allocation.
  // Write some data to that slot.
  vp[0] = 0xDE;
  vp[1] = 0xAD;
  vp[2] = 0xBE;
  vp[3] = 0xEF;

  // Create a default-initialized array in the pool
  auto int2 = f.CreateForOverwrite();

  // The data we wrote before construction is still there. No zero initialization occurred.
  EXPECT_EQ(int2->at(0), 0xDE);
  EXPECT_EQ(int2->at(1), 0xAD);
  EXPECT_EQ(int2->at(2), 0xBE);
  EXPECT_EQ(int2->at(3), 0xEF);
}

} // namespace storage_pool
} // namespace pando
} // namespace pnd
