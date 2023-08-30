#include "mutex.h"

#include "gtest/gtest.h"

#include <future>
#include <memory>

namespace pnd {
namespace pando {

/** Helper function to check if g_hdf5_mutex is unlocked.
 * Since g_hdf5_mutex is a std::recursive_mutex, we must do this from another thread in order to
 * actually contend.
 */
bool LockIsAvailable() {
  return std::async(
             std::launch::async,
             []() {
               std::unique_lock<std::recursive_mutex> l{g_hdf5_mutex, std::try_to_lock};
               return l.owns_lock();
             })
      .get();
}

/** A stand-in for some class that calls the non-threadsafe H5 library during construction and
 * destruction. */
struct Unsafe {
  /** Default constructor. */
  Unsafe() {
    EXPECT_FALSE(LockIsAvailable());
    construct_count++;
  }

  /** Copy constructor. */
  Unsafe(const Unsafe&) {
    EXPECT_FALSE(LockIsAvailable());
    copy_construct_count++;
  }

  /** Move constructor. */
  Unsafe(Unsafe&&) {
    EXPECT_FALSE(LockIsAvailable());
    move_construct_count++;
  }

  /** Destructor. */
  ~Unsafe() {
    EXPECT_FALSE(LockIsAvailable());
    destruct_count++;
  }

  /** Copy assignment operator. */
  Unsafe& operator=(const Unsafe&) {
    EXPECT_FALSE(LockIsAvailable());
    copy_assign_count++;
    return *this;
  }

  /** Move assignment operator. */
  Unsafe& operator=(Unsafe&&) {
    EXPECT_FALSE(LockIsAvailable());
    move_assign_count++;
    return *this;
  }

  static inline int construct_count = 0;
  static inline int copy_construct_count = 0;
  static inline int move_construct_count = 0;
  static inline int destruct_count = 0;
  static inline int copy_assign_count = 0;
  static inline int move_assign_count = 0;
};

/** By wrapping Unsafe in H5LockAdapter, we make it Safe. */
using Safe = H5LockAdapter<Unsafe>;

/** Test that g_hdf5_mutex is locked/unlocked when expected. */
TEST(H5LockAdapterTest, Sequencing) {
  // Lock starts off available
  ASSERT_TRUE(LockIsAvailable());

  // We can serially construct multiple instances of Safe.
  // The lock is obtained and released for each construction.
  auto s1 = std::make_unique<Safe>();
  ASSERT_TRUE(LockIsAvailable());
  auto s2 = std::make_unique<Safe>();
  ASSERT_TRUE(LockIsAvailable());
  auto s3 = std::make_unique<Safe>();
  ASSERT_TRUE(LockIsAvailable());

  // Constructor body indeed ran 3 times
  EXPECT_EQ(Unsafe::construct_count, 3);

  // Copy constructor is invoked safely
  Safe s_copy_constructed = *s1;
  EXPECT_EQ(Unsafe::copy_construct_count, 1);

  // Move constructor is invoked safely
  Safe s_move_constructed = std::move(s_copy_constructed);
  EXPECT_EQ(Unsafe::move_construct_count, 1);

  // Copy assignment operator is invoked safely
  Safe s_copy_assigned;
  s_copy_assigned = *s1;
  EXPECT_EQ(Unsafe::copy_assign_count, 1);

  // Move assignment operator is invoked safely
  Safe s_move_assigned;
  s_move_assigned = std::move(s_copy_assigned);
  EXPECT_EQ(Unsafe::move_assign_count, 1);

  // We can serially destruct multiple instances of Safe.
  // The lock is obtained and released for each destruction.
  s1.reset();
  ASSERT_TRUE(LockIsAvailable());
  s2.reset();
  ASSERT_TRUE(LockIsAvailable());
  s3.reset();
  ASSERT_TRUE(LockIsAvailable());

  // Destructor body indeed ran 3 times
  EXPECT_EQ(Unsafe::destruct_count, 3);
}

/** Prove our LockIsAvailable helper function behaves as expected. */
TEST(LockIsAvailable, Sanity) {
  // Returns true if lock is not owned by main thread
  ASSERT_TRUE(LockIsAvailable());

  // Returns false if lock is owned by main thread
  {
    H5LockGuard l;
    ASSERT_FALSE(LockIsAvailable());
  }

  // Returns true again after main thread gives up lock
  ASSERT_TRUE(LockIsAvailable());
}

} // namespace pando
} // namespace pnd
