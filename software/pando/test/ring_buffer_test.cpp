#include "ring_buffer.h"

#include <chrono>
#include <cstdint>
#include <thread>

#include <iostream>

#include "gtest/gtest.h"

namespace pnd {
namespace pando {

/** Construct a RingBuffer instance. */
TEST(RingBufferTest, Initialization) {
  RingBuffer<uint8_t, 3> b;
}

/** Test that the buffer appears empty upon initialization. */
TEST(RingBufferTest, StartsEmpty) {
  RingBuffer<uint8_t, 3> b;

  EXPECT_EQ(b.AdvanceRead(), nullptr);
}

/** Test that the buffer appears empty after calling Reset. */
TEST(RingBufferTest, ResetEmpty) {
  RingBuffer<uint8_t, 3> b;

  // Fill it up
  b.AdvanceWrite();
  b.AdvanceWrite();

  b.Reset();

  // Check read pointer was reset
  EXPECT_EQ(b.AdvanceRead(), nullptr);

  // Check write pointer was reset
  EXPECT_TRUE(b.MaybeAdvanceWrite());
  EXPECT_TRUE(b.MaybeAdvanceWrite());
}

/** Test that a buffer with 3 slots overflows after exactly 3 writes */
TEST(RingBufferTest, Capacity3) {
  RingBuffer<uint8_t, 3> b;

  for (int i = 0; i < 3; ++i) {
    EXPECT_TRUE(b.AdvanceWrite());
  }
  EXPECT_FALSE(b.MaybeAdvanceWrite());
  EXPECT_THROW(b.AdvanceWrite(), std::overflow_error);
}

/** Test that a buffer with 99 slots overflows after exactly 99 writes */
TEST(RingBufferTest, Capacity99) {
  RingBuffer<uint8_t, 99> b;

  for (int i = 0; i < 99; ++i) {
    EXPECT_TRUE(b.AdvanceWrite());
  }
  EXPECT_FALSE(b.MaybeAdvanceWrite());
  EXPECT_THROW(b.AdvanceWrite(), std::overflow_error);
}

/** Test that buffers are presented to the reader in the same order as they are produced by the
 * writer */
TEST(RingBufferTest, FIFO3) {
  RingBuffer<uint8_t, 3> b;

  auto i1 = b.AdvanceWrite();
  auto i2 = b.AdvanceWrite();
  auto i3 = b.AdvanceWrite();

  EXPECT_EQ(b.AdvanceRead(), i1);
  EXPECT_EQ(b.AdvanceRead(), i2);
  EXPECT_EQ(b.AdvanceWrite(), i1);
  EXPECT_EQ(b.AdvanceRead(), i3);
  EXPECT_EQ(b.AdvanceRead(), nullptr);
}

/** Class that implements a multi threaded producer and consumer for use in the next tests.
 *
 * The producer stuffs length (128 * 4096 + 1) arrays with sequential numbers. The consumer checks
 * that each elemnent in each item is as expected. Critically, the produce step takes longer than
 * the consume step, so the buffer never overflows.
 *
 * @tparam N The number of slots in the RingBuffer
 */
template <size_t N>
class ProducerConsumer {
 public:
  void Run() {
    std::thread producer([&] { ASSERT_NO_THROW(Produce()); });

    Consume();

    producer.join();
  }

 private:
  void Produce() {
    for (size_t i = 0; i < kNumBuffers; ++i) {
      uint8_t* buff;

      do {
        buff = b_.MaybeAdvanceWrite();
      } while (buff == nullptr);

      for (size_t j = 0; j < kBuffSize; ++j) {
        buff[j] = static_cast<typename decltype(b_)::ItemElement>(i * kBuffSize + j);
      }
    }

    // Commit the last write
    b_.AdvanceWrite();
  }

  void Consume() {
    // Check that the first byte of each buffer is as expected
    for (size_t i = 0; i < kNumBuffers; ++i) {
      const uint8_t* buff;
      do {
        buff = b_.AdvanceRead();
      } while (!buff);

      EXPECT_EQ(buff[0], static_cast<uint8_t>(i * kBuffSize));
    }
  }

  static constexpr size_t kNumBuffers = 8192;
  static constexpr size_t kBuffSize = 128 * 4096 + 1;

  // Note: Buffer has N slots.
  // Note: Item is an array type, there's kBuffSize elements in each Item.
  RingBuffer<uint8_t[], N> b_{kBuffSize};
};

/** Multithreaded producer and consumer with three slots */
TEST(ProducerConsumerTest, ProduceConsume3) {
  ProducerConsumer<3> pc;
  pc.Run();
}

/** Multithreaded producer and consumer with five slots */
TEST(ProducerConsumerTest, ProduceConsume5) {
  ProducerConsumer<5> pc;
  pc.Run();
}

} // namespace pando
} // namespace pnd
