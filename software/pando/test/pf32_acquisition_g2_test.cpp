#include "pf32_acquisition_g2.h"
#include "pf32_ll.h"
#include "reporter.h"
#include "thread_prio.h"

#include <cstdint>

#include "gtest/gtest.h"

#include <chrono>
#include <list>
#include <memory>
#include <thread>

namespace pnd {
namespace pando {
namespace pf32_ll {

/** Test which frame rates are achieveable without dropping frames.
 * If we don't drop any frames, the frame_count field in each succesive frame will always increment
 * by one.
 */
class AcquisitionG2Test : public ::testing::Test {
 protected:
  AcquisitionG2Test() {
    // Populate enabled_channels_ with 0-1023
    for (int32_t ch = 0; ch < 1023; ++ch) {
      enabled_channels_.push_back(ch);
    }
  }

  static constexpr size_t kNReads = 100;

  std::list<int32_t> enabled_channels_;

  // Assumes we're running from the root of ovrsource
  PF32LL pf32_ll_{"./third-party/pf32_api/pf32_api/bitfiles/PF32_USBC_corr_64tau_[2896].bit"};
  std::unique_ptr<pf32_ll::AcquisitionG2> acquisition_;
};

/** Construct a PF32LL instance. */
TEST_F(AcquisitionG2Test, DISABLED_Initialization) {}

/** Make sure first_frame fields come roughly as expected.
 * There is some dead time between correlations.
 */
TEST_F(AcquisitionG2Test, DISABLED_CreateAcquisition) {
  acquisition_ = pf32_ll_.CreateAcquisitionG2(
      std::chrono::nanoseconds{1600}, 6400, false, {1, 1, 1, 1}, enabled_channels_);

  size_t last_begin_frame_idx = 0;
  for (int i = 0; i < 100; ++i) {
    auto result = acquisition_->GetResult();
    if (last_begin_frame_idx) {
      auto delta = result->begin_frame_idx - last_begin_frame_idx;
      EXPECT_GE(delta, 6400);
      EXPECT_LT(delta, 6500);
    }
    last_begin_frame_idx = result->begin_frame_idx;
  }
}

/** Make sure that multiple experiments won't trigger "dropped frames" exception. */
TEST_F(AcquisitionG2Test, DISABLED_RepeatedStart) {
  acquisition_ = pf32_ll_.CreateAcquisitionG2(
      std::chrono::nanoseconds{1600}, 6400, false, {1, 1, 1, 1}, enabled_channels_);

  for (int i = 0; i < 100; ++i) {
    auto result = acquisition_->GetResult();
  }

  acquisition_.reset();

  std::this_thread::sleep_for(std::chrono::seconds(1));

  acquisition_ = pf32_ll_.CreateAcquisitionG2(
      std::chrono::nanoseconds{1600}, 6400, false, {1, 1, 1, 1}, enabled_channels_);

  for (int i = 0; i < 100; ++i) {
    auto result = acquisition_->GetResult();
  }
}

/** Make sure the tau values for rebin factors {1,1,1,1} match the documentation. */
TEST_F(AcquisitionG2Test, DISABLED_CheckTau1111) {
  acquisition_ = pf32_ll_.CreateAcquisitionG2(
      std::chrono::nanoseconds{1600}, 6400, false, {1, 1, 1, 1}, enabled_channels_);

  // Get a single result
  auto result = acquisition_->GetResult();

  static constexpr std::array<int, 64> kExpectedTaus = {
      1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
      23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
      45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
  };

  for (int k = 0; k < result->tau_k.size(); ++k)
    EXPECT_EQ(
        result->tau_k.at(k).count(),
        std::chrono::round<decltype(result->tau_k)::value_type>(
            kExpectedTaus.at(k) * acquisition_->GetFramePeriod())
            .count());
}

/** Make sure the tau values for rebin factors {8,8,8,8} match the documentation. */
TEST_F(AcquisitionG2Test, DISABLED_CheckTau8888) {
  acquisition_ = pf32_ll_.CreateAcquisitionG2(
      std::chrono::nanoseconds{1600}, 6400, false, {8, 8, 8, 8}, enabled_channels_);

  // Get a single result
  auto result = acquisition_->GetResult();

  static constexpr std::array<int, 64> kExpectedTaus = {
      1,    2,    3,    4,    5,    6,     7,     8,     9,     17,    25,    33,    41,
      49,   57,   65,   73,   81,   89,    97,    105,   113,   121,   129,   137,   201,
      265,  329,  393,  457,  521,  585,   649,   713,   777,   841,   905,   969,   1033,
      1097, 1161, 1673, 2185, 2697, 3209,  3721,  4233,  4745,  5257,  5769,  6281,  6793,
      7305, 7817, 8329, 8841, 9353, 13449, 17545, 21641, 25737, 29833, 33929, 38025,
  };

  for (int k = 0; k < result->tau_k.size(); ++k)
    EXPECT_EQ(
        result->tau_k.at(k).count(),
        std::chrono::round<decltype(result->tau_k)::value_type>(
            kExpectedTaus.at(k) * acquisition_->GetFramePeriod())
            .count());
}

/** Test fixture for AcquisitionG2::ConvertCorrelatorOutput.
 * It is intended to perfrom the exact same calculation as the convertCorrelatorOutput method from
 * the correlator API, but is faster due to the fact that we build with AVX2 instructions.
 */
class ConvertCorrelatorOutputBench : public ::testing::Test {
 protected:
  static constexpr std::chrono::seconds kRunTime{1};

  ConvertCorrelatorOutputBench() {
    raw_result = std::make_unique<AcquisitionG2::RawCorrelatorResult>();

    // Fill the source with the same random garbage every time
    std::srand(1);
    for (auto& b : raw_result->correlator_data) {
      b = std::rand();
    }

    // Raise thread to highest possible priority for benchmarking
    common::thread_prio::SetProcessPriorityClass(common::thread_prio::PriorityClass::kRealtime);
    common::thread_prio::SetThreadPrio(common::thread_prio::ThreadPriority::kTimeCritical);
  }

  ~ConvertCorrelatorOutputBench() {
    // Reset to normal priority
    common::thread_prio::SetProcessPriorityClass(common::thread_prio::PriorityClass::kNormal);
    common::thread_prio::SetThreadPrio(common::thread_prio::ThreadPriority::kNormal);
  }

  std::unique_ptr<AcquisitionG2::RawCorrelatorResult> raw_result;
  std::unique_ptr<AcquisitionG2::CorrelatorResult> result;
};

// convertCorrelatorOutput takes a PF32_HANDLE as its first argument, but from inspecting the dll
// it seems all it does with this handle is read a single uint32 at an offset of 0x168. This is the
// number of tau values per pixel, which is either 16 32 or 64 depending on the loaded bitfile.
//
// WARNING: this is fragile and subject to breakage if the internal ABI changes.
struct MOCKPF32_HANDLE {
  uint32_t _pad[90]{};
  uint32_t n_points{64};
};

/** Benchmark convertCorrelatorOutput from the correlator API. */
TEST_F(ConvertCorrelatorOutputBench, BenchTheirs) {
  MOCKPF32_HANDLE mock_pf32_handle{};

  auto result = std::make_unique<AcquisitionG2::CorrelatorResult>();
  auto stop_time = std::chrono::steady_clock::now() + kRunTime;
  int iter_cnt;
  for (iter_cnt = 0; std::chrono::steady_clock::now() < stop_time; ++iter_cnt) {
    api::convertCorrelatorOutput(
        &mock_pf32_handle,
        raw_result->correlator_data.data(),
        result->g2[0][0].data(),
        kRowColCount,
        kRowColCount);
  }
  g_reporter->info("Theirs ran {} iterations.", iter_cnt);
}

/** Benchmark  AcquisitionG2::ConvertCorrelatorOutput. */
TEST_F(ConvertCorrelatorOutputBench, BenchOurs) {
  auto result = std::make_unique<AcquisitionG2::CorrelatorResult>();
  auto stop_time = std::chrono::steady_clock::now() + kRunTime;
  int iter_cnt;
  for (iter_cnt = 0; std::chrono::steady_clock::now() < stop_time; ++iter_cnt) {
    AcquisitionG2::ConvertCorrelatorOutput(
        raw_result->correlator_data.data(), result->g2[0][0].data(), kRowColCount, kRowColCount);
  }
  g_reporter->info("Ours ran {} iterations.", iter_cnt);
}

/** Verify that both implementations produce identical results. */
TEST_F(ConvertCorrelatorOutputBench, TestOurs) {
  MOCKPF32_HANDLE mock_pf32_handle{};

  auto our_result = std::make_unique<AcquisitionG2::CorrelatorResult>();
  AcquisitionG2::ConvertCorrelatorOutput(
      raw_result->correlator_data.data(), our_result->g2[0][0].data(), kRowColCount, kRowColCount);

  auto their_result = std::make_unique<AcquisitionG2::CorrelatorResult>();
  api::convertCorrelatorOutput(
      &mock_pf32_handle,
      raw_result->correlator_data.data(),
      their_result->g2[0][0].data(),
      kRowColCount,
      kRowColCount);

  // Array sizes
  using G2 = decltype(our_result->g2);
  static constexpr std::size_t kRowCount = std::tuple_size<G2>::value;
  static constexpr std::size_t kTauCount = std::tuple_size<G2::value_type>::value;
  static constexpr std::size_t kColCount = std::tuple_size<G2::value_type::value_type>::value;

  for (std::size_t row_idx = 0; row_idx < kRowCount; ++row_idx) {
    for (std::size_t tau_idx = 0; tau_idx < kTauCount; ++tau_idx) {
      for (std::size_t col_idx = 0; col_idx < kColCount; ++col_idx) {
        EXPECT_EQ(
            our_result->g2[row_idx][tau_idx][col_idx], their_result->g2[row_idx][tau_idx][col_idx]);
      }
    }
  }
}

} // namespace pf32_ll
} // namespace pando
} // namespace pnd
