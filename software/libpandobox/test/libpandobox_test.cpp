#include "pando_box_factory.h"

#include "dma.h"

#include <stdio.h>
#include "gtest/gtest.h"

#include <chrono>
#include <thread>

namespace pnd {
namespace libpandobox {

/** Construct a PandoBox instance. */
TEST(PandoBoxTest, Construction) {
  auto h = PandoBoxFactory::Create(false);
}

/** Run with no peripherals enabled for 10 seconds*/
TEST(PandoBoxTest, Run10s) {
  auto h = PandoBoxFactory::Create(false);

  h->SetRun(true);
  std::this_thread::sleep_for(std::chrono::seconds{10});
  h->SetRun(false);
}

/** Run with nothing enabled and make sure we exit as expected
 */
TEST(PandoBoxTest, ReadDmaTimeout) {
  auto h = PandoBoxFactory::Create(false);

  char input_buf[4096] = {0};
  auto amt_read = h->Read(input_buf, 4096, 1000);

  ASSERT_EQ(amt_read, 0);
}

/** Run with traffic gen enabled for 1 seconds and make sure we can
 * read from the DMA controller and that it is streaming data at
 * the correct rate **/
TEST(PandoBoxTest, TrafficGenDma) {
  struct TrafficGenSample {
    uint32_t sample_type;
    uint32_t sample_size;
    uint64_t sample_time;
    uint16_t device_id;
    uint16_t _reserved;
  } __attribute__((__packed__));

  std::srand(std::time(0));
  auto h = PandoBoxFactory::Create(false);
  h->StartDma();

  // Set sample rate to 1k sample/second
  h->SetTrgen0SampRateDiv(100000);
  h->SetTrgen0Enabled(true);
  h->SetRun(true);

  auto begin = std::chrono::steady_clock::now();
  auto delta =
      std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - begin)
          .count();

  char input_buf[4096] = {0};

  int total_read = 0;
  while (delta < 1) {
    // Sleep for up to 30 ms to ensure we get variable packet read amounts
    auto sleep_amt = std::rand() % 30;
    std::this_thread::sleep_for(std::chrono::milliseconds{sleep_amt});

    auto amt_read = h->Read(input_buf, 4096, 1000);
    total_read += amt_read;
    if (!amt_read) {
      continue;
    }
    for (int i = 0; i < amt_read; i += sizeof(TrafficGenSample)) {
      TrafficGenSample* sample = reinterpret_cast<TrafficGenSample*>(&input_buf[i]);
      ASSERT_EQ(sample->sample_type, static_cast<uint32_t>(sample_format::Magic::kTrafficGen));
      ASSERT_EQ(sample->sample_size, sizeof(TrafficGenSample));
    }
    delta =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - begin)
            .count();
  }
  h->SetTrgen0Enabled(false);
  h->SetRun(false);
  h->StopDma();

  // If we're streaming at 1ksample/second, we should be pretty darn
  // close.  Given that we have random delays, the amount can swing a bit
  ASSERT_GE(total_read / sizeof(TrafficGenSample), 900UL);
  ASSERT_LE(total_read / sizeof(TrafficGenSample), 1100UL);
}

/** Run with traffic gen enabled for 1 seconds and make sure we can
 * read from the DMA controller using the zero-copy read interface
 */
TEST(PandoBoxTest, PandoBoxRead2) {
  std::srand(std::time(0));

  auto h = PandoBoxFactory::Create(false);
  h->StartDma();

  // Set sample rate to 1k sample/second
  h->SetTrgen0SampRateDiv(100000);
  h->SetTrgen0Enabled(true);
  h->SetRun(true);

  auto begin = std::chrono::steady_clock::now();
  auto delta =
      std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - begin)
          .count();

  unsigned int total_read = 0;
  while (delta < 1) {
    // Sleep for up to 30 ms to ensure we get variable packet read amounts
    auto sleep_amt = std::rand() % 30;
    std::this_thread::sleep_for(std::chrono::milliseconds{sleep_amt});

    h->ConsumeSamples(
        [&total_read](auto samples) {
          for (auto sample : samples) {
            ASSERT_EQ(sample->type, static_cast<uint32_t>(sample_format::Magic::kTrafficGen));
            ASSERT_EQ(sample->size, 0x14);
          }
          total_read += samples.size();
        },
        128,
        1000);

    delta =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - begin)
            .count();
  }
  h->SetTrgen0Enabled(false);
  h->SetRun(false);
  h->StopDma();

  // If we're streaming at 1ksample/second, we should be pretty darn
  // close.  Given that we have random delays, the amount can swing a bit
  ASSERT_GE(total_read, 900UL);
  ASSERT_LE(total_read, 1100UL);
}

/** Run with traffic gen enabled but without consuming the resulting data.
 * Should trigger an overflow exception. in ~4.096 seconds.
 */
TEST(PandoBoxTest, DmaOverflow) {
  auto h = PandoBoxFactory::Create(false);
  h->StartDma();

  auto begin = std::chrono::steady_clock::now();

  // Set sample rate to 1k sample/second
  h->SetTrgen0SampRateDiv(100000);
  h->SetTrgen0Enabled(true);
  h->SetRun(true);

  auto time_to_overflow = std::chrono::milliseconds{1} * libpandobox::AxiDma::kMaxDescriptors;
  constexpr std::chrono::milliseconds poll_interval{100};

  // Perform 0 length reads (doesn't move tail descriptor) to verify no overflow has occured
  while (std::chrono::steady_clock::now() < begin + time_to_overflow - poll_interval) {
    h->Read(nullptr, 0, 0);
    std::this_thread::sleep_for(poll_interval);
  }

  // Wait until after an overflow should have occured, then attempt one more read.
  std::this_thread::sleep_until(begin + time_to_overflow + poll_interval);
  EXPECT_THROW(h->Read(nullptr, 0, 0), std::runtime_error);

  h->SetTrgen0Enabled(false);
  h->SetRun(false);
  h->StopDma();
}

} // namespace libpandobox
} // namespace pnd
