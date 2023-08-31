#include "worker.h"

#include <chrono>
#include <functional>
#include <stdexcept>
#include <thread>
#include <utility>

#include "gtest/gtest.h"

namespace pnd {
namespace pando {

class WorkerTest : public ::testing::Test {
 protected:
  /** Collection of descriptively named member functions */
  struct Funcs {
    static int Static(int i) {
      return i + 1;
    }

    int Member(int i) {
      return i + 1;
    }

    int ConstMember(int i) const {
      return i + 1;
    }

    static void Throwing() {
      throw std::runtime_error("Intentional Exception");
    }
  };

  /** Functor (with two operator() overloads) */
  struct Functor {
    int operator()(int i) {
      return i + 1;
    }

    double operator()(double f) {
      return f + 0.1;
    }
  };

  /** Functor that cannot be default constructed, copied, or assigned to. */
  struct RestrictiveFunctor {
    void operator()() {}

    // Explicitly define special member functions
    RestrictiveFunctor(int) {} // NOT default constructable (must provide an int argument)
    RestrictiveFunctor(RestrictiveFunctor&&) = default; // Move constructable
    RestrictiveFunctor(const RestrictiveFunctor&) = delete; // NOT copy constructable
    RestrictiveFunctor& operator=(const RestrictiveFunctor&) = delete; // NOT copy assignable
    RestrictiveFunctor& operator=(RestrictiveFunctor&&) = delete; // NOT move assignable
  };

  /** Functor that returns the number of times it has previously been called. */
  struct StatefulFunctor {
    int operator()() {
      return state++;
    }

    int state = 0;
  };

  /** The Worker instance used in every test */
  Worker w_{"worker"};
};

/** Create a worker */
TEST_F(WorkerTest, Initialize){};

/** Run a static member function in a worker */
TEST_F(WorkerTest, RunStatic) {
  std::future<int> f = w_.Async(Funcs::Static, 0);

  ASSERT_TRUE(f.valid());
  EXPECT_EQ(f.get(), 1);
};

/** Run a static member function in a worker 1000 times */
TEST_F(WorkerTest, RunStatic1000) {
  std::future<int> f = w_.Async(Funcs::Static, 0);
  for (int i = 1; i < 1000; ++i) {
    f = w_.Async(Funcs::Static, f.get());
  }

  ASSERT_TRUE(f.valid());
  EXPECT_EQ(f.get(), 1000);
};

/** Run a member function in a worker */
TEST_F(WorkerTest, RunMember) {
  // The class instance we will run the member function on (ie. "this")
  Funcs instance;

  std::future<int> f = w_.Async(&Funcs::Member, &instance, 0);

  ASSERT_TRUE(f.valid());
  EXPECT_EQ(f.get(), 1);
};

/** Run a const member function in a worker */
TEST_F(WorkerTest, RunConstMember) {
  // The class instance we will run the member function on (ie. "this")
  const Funcs instance;

  std::future<int> f = w_.Async(&Funcs::ConstMember, &instance, 0);

  ASSERT_TRUE(f.valid());
  EXPECT_EQ(f.get(), 1);
};

/** Catch an exception thrown in the worker. */
TEST_F(WorkerTest, RunThrowing) {
  std::future<void> f = w_.Async(Funcs::Throwing);

  ASSERT_TRUE(f.valid());
  EXPECT_THROW(f.get(), std::runtime_error);
};

/** Run a Functor (overload #1) */
TEST_F(WorkerTest, RunFunctorOverload1) {
  // Providing int argument causes first overload to be used
  std::future<int> f = w_.Async(Functor{}, 0);

  ASSERT_TRUE(f.valid());
  EXPECT_EQ(f.get(), 1);
};

/** Run a Functor (overload #2) */
TEST_F(WorkerTest, RunFunctorOverload2) {
  // Providing double argument causes second overload to be used
  std::future<double> f = w_.Async(Functor{}, 0.0);

  ASSERT_TRUE(f.valid());
  EXPECT_EQ(f.get(), 0.1);
};

/** Prove that an rvalue functor is never copied */
TEST_F(WorkerTest, RunRestrictiveFunctor) {
  // Providing int argument causes first overload to be used
  std::future<void> f = w_.Async(RestrictiveFunctor{0});

  ASSERT_TRUE(f.valid());
  f.get();
};

/** Run a copy of a stateful functor */
TEST_F(WorkerTest, RunStatefulFunctor1) {
  StatefulFunctor func;

  // Inside Worker::Async, std::async makes a copy of func
  std::future<int> f = w_.Async(func);
  ASSERT_TRUE(f.valid());
  EXPECT_EQ(f.get(), 0);

  // Original func hasn't been called yet, so should still return 0
  EXPECT_EQ(func(), 0);

  // But now that we've called it once, should return 1
  EXPECT_EQ(func(), 1);
};

/** Run a Stateful functor */
TEST_F(WorkerTest, RunStatefulFunctor2) {
  StatefulFunctor func;

  // Wrapping func in a std::reference_wrapper causes std::async to store a reference to the
  // original instead of making a copy
  std::future<int> f = w_.Async(std::ref(func));
  ASSERT_TRUE(f.valid());
  EXPECT_EQ(f.get(), 0);

  // Original func has been called, so should return 1
  EXPECT_EQ(func(), 1);
};

/** Queue up two tasks, prove that the second is queued before the first finishes. */
TEST_F(WorkerTest, Recursive) {
  // First task just sleeps for 100ms
  std::future<void> f0 = w_.Async(
      std::this_thread::
          sleep_for<std::chrono::milliseconds::rep, std::chrono::milliseconds::period>,
      std::chrono::milliseconds(100));
  ASSERT_TRUE(f0.valid());

  // Second task that waits for the first one to complete
  std::future<void> f1 = w_.Async(&std::future<void>::wait, &f0);
  ASSERT_TRUE(f1.valid());

  // We returned fom the second call to Worker::Async even though the first task isn't done
  EXPECT_EQ(f0.wait_for(std::chrono::milliseconds(0)), std::future_status::timeout);

  // The second future takes ~100ms to become ready
  EXPECT_EQ(f1.wait_for(std::chrono::milliseconds(0)), std::future_status::timeout);
  EXPECT_EQ(f1.wait_for(std::chrono::milliseconds(200)), std::future_status::ready);
};

} // namespace pando
} // namespace pnd
