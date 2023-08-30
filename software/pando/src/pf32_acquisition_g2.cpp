#include "pf32_acquisition_g2.h"

#include "pf32_ll_types.h"
#include "reporter.h"
#include "thread_prio.h"
#include "worker.h"

#include <algorithm>
#include <cassert>
#include <functional>

namespace pnd {
namespace pando {
namespace pf32_ll {

AcquisitionG2::AcquisitionG2(
    Handle handle,
    std::chrono::nanoseconds frame_period,
    unsigned long frame_count,
    bool burst_mode,
    std::array<int, 4> rebin_factors,
    const std::list<int32_t>& enabled_channels)
    : handle_(handle), frame_count_{frame_count}, rebin_factors_{rebin_factors} {
  n_points_ = api::getNoOfTauValuesPerPixel(handle_.get());
  g_reporter->debug("pf32_ll::AcquisitionG2: this firmware has n_pixels = {}", n_points_);

  for (auto factor : rebin_factors)
    switch (factor) {
      case 1:
      case 2:
      case 4:
      case 8:
        break;
      default:
        throw Exception("all rebin_factors must be 1, 2, 4, or 8.");
    }

  // Set the mode
  api::setMode(handle_.get(), api::photon_counting);

  // Enable intensity map
  api::enableIntensityMap(handle_.get(), true);

  // Set the frame period
  if (frame_period < std::chrono::nanoseconds{2000})
    throw std::runtime_error(
        "pf32_ll::AcquisitionG2: requested frame period is too short (minimum is 2000ns)");
  auto frame_period_us =
      std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(frame_period);
  api::setExposure_us(handle_.get(), frame_period_us.count());
  api::setFramesToSum(handle_.get(), 1);

  // Read back the frame period
  frame_period_ps_ = std::chrono::round<picoseconds>(
      std::chrono::duration<double, std::micro>{api::getExposure_us(handle_.get())});
  if (frame_period_ps_ != frame_period) {
    g_reporter->debug(
        "pf32_ll::AcquisitionG2: Reported frame period is {}ps, not {}ns as requested",
        frame_period_ps_.count(),
        frame_period.count());
    throw Exception("pf32_ll::AcquisitionG2: Requested frame period was not achieved");
  }

  if (burst_mode) {
    // enable burst mode
    api::SetWireInValue(handle_.get(), 0x18, (1 << 26), (1 << 26));
    // use FRM as trigger
    api::SetWireInValue(handle_.get(), 0xE, (1 << 29), (0x3 << 29));

  } else {
    // disable burst mode
    api::SetWireInValue(handle_.get(), 0x18, (0 << 26), (1 << 26));
  }
  api::UpdateWireIns(handle_.get());

  // Select enabled pixels
  // TODO correlator firmware does not support disabiing channels. Implement once it does.
  (void)enabled_channels;
  if (api::getEnabledHeight(handle_.get()) != kRowColCount) {
    throw Exception("getEnabledHeight didn't return kRowColCount as expected.");
  }

  enabled_height_ = api::getEnabledHeight(handle_.get());

  // Sanity checks
  assert(enabled_height_ == kRowColCount);
  assert(api::getWidth(handle_.get()) == kRowColCount);
  assert(api::getHeight(handle_.get()) == kRowColCount);

  // Set the integration period
  api::setTintFrames(handle_.get(), frame_count_);

  // Configure exponential rebinning
  // Each call to setSpacing sets the rebinning factor for all points in the same "rebinning region"
  // as the rebinningPoint argument. kRebinningIndices holds the zero-based index of the first point
  // in each region. Note theat setSpacing() takes a one-based index.
  for (std::size_t i = 0; i < kRebinningIndices.size(); ++i) {
    if (kRebinningIndices[i] < n_points_) // 32 tau firmware only has 2 rebinning points
      api::setSpacing(handle_.get(), rebin_factors[i], kRebinningIndices[i] + 1);
  }

  // Spew a bunch of debug information
  g_reporter->debug(
      "pf32_ll::AcquisitionG2: getSizeOfCorrelatorData -> {}",
      api::getSizeOfCorrelatorData(handle_.get()));
  g_reporter->debug(
      "pf32_ll::AcquisitionG2: getSensorClk_Hz -> {}", api::getSensorClk_Hz(handle_.get()));
  g_reporter->debug("pf32_ll::AcquisitionG2: getSync_Hz -> {}", api::getSync_Hz(handle_.get()));
  g_reporter->debug(
      "pf32_ll::AcquisitionG2: getLinkStatus -> {}", api::getLinkStatus(handle_.get()));
  g_reporter->debug(
      "pf32_ll::AcquisitionG2: getExposure_us -> {}", api::getExposure_us(handle_.get()));
  g_reporter->debug(
      "pf32_ll::AcquisitionG2: getSensorClk_Hz -> {}", api::getSensorClk_Hz(handle_.get()));
  g_reporter->debug(
      "pf32_ll::AcquisitionG2: getBitsPerLine -> {}", api::getBitsPerLine(handle_.get()));
  g_reporter->debug(
      "pf32_ll::AcquisitionG2: getLinesPerFrame -> {}", api::getLinesPerFrame(handle_.get()));
  g_reporter->debug(
      "pf32_ll::AcquisitionG2: getNoOfPixels -> {}", api::getNoOfPixels(handle_.get()));
  g_reporter->debug(
      "pf32_ll::AcquisitionG2: getFramesToSum -> {}", api::getFramesToSum(handle_.get()));
  g_reporter->debug(
      "pf32_ll::AcquisitionG2: getNoOfFramesInBuffer -> {}",
      api::getNoOfFramesInBuffer(handle_.get()));
  g_reporter->debug(
      "pf32_ll::AcquisitionG2: getSPADEnable -> {}", api::getSPADEnable(handle_.get()));
  auto serial_number = std::make_unique<char[]>(api::MAX_SERIAL_NUMBER_LENGTH);
  api::GetSerialNumber(handle_.get(), serial_number.get());
  g_reporter->debug("pf32_ll::AcquisitionG2: getSerialNumber -> {}", serial_number.get());
  g_reporter->debug(
      "pf32_ll::AcquisitionG2: getNoOfTDCCodes -> {}", api::getNoOfTDCCodes(handle_.get()));
  g_reporter->debug("pf32_ll::AcquisitionG2: getBitMode -> {}", api::getBitMode(handle_.get()));
  g_reporter->debug("pf32_ll::AcquisitionG2: getNoOfTauValuesPerPixel -> [{}]", n_points_);

  common::thread_prio::SetProcessPriorityClass(common::thread_prio::PriorityClass::kRealtime);

  // Launch acquisition thread
  acquisition_thread_ = common::ThreadContainer(
      [&] { Acquire(); },
      "pf32_ll::AcquisitionG2::Acquire",
      &acquisition_stop_signal_,
      common::thread_prio::ThreadPriority::kTimeCritical);
}

storage_pool::FactoryHandle<AcquisitionG2::CorrelatorResult> AcquisitionG2::GetResult() {
  std::future<storage_pool::FactoryHandle<CorrelatorResult>> fut;

  // Wait until a new future is available
  auto max_delay = frame_period_ps_ * frame_count_ + kMaxExcessDelay;
  {
    std::unique_lock<std::mutex> l{result_futures_m_};
    result_futures_cv_.wait_for(l, max_delay, [&] { return !result_futures_.empty(); });

    if (result_futures_.empty()) {
      throw Exception("GetResult: blocked for >500ms longer than requested Tint.");
    }

    fut = std::move(result_futures_.front());
    result_futures_.pop();
  }

  // Wait until the future holds a result
  return fut.get();
}

/** Thread function that continuously acquires raw correlator results from the PF32. */
void AcquisitionG2::Acquire() {
  auto correlator_data_size = api::getSizeOfCorrelatorData(handle_.get());
  g_reporter->debug("correlator_data_size={}", correlator_data_size);
  if (correlator_data_size != RawCorrelatorResult::kMaxCorrelatorDataSize) {
    throw Exception("getSizeOfCorrelatorData did not return expected value.");
  }

  // Worker for async processing of raw data
  Worker process_worker_{"pf32_ll::AcquisitionG2::Acquire::::process_worker_",
                         common::thread_prio::ThreadPriority::kAboveNormal};

  // Handle that enables the correlator and then eventually disables it upon thread exit
  struct CapStopper {
    CapStopper(Handle& handle) : handle_{handle} {
      api::enableCorrelator(handle_.get(), true, false);
    }
    ~CapStopper() {
      api::enableCorrelator(handle_.get(), false, false);
    }
    Handle handle_;
  } cap_stopper{handle_};

  // The first two frames returned by readFromCorrelator are corrupt
  auto garbage = raw_results_storage_pool_.CreateForOverwrite();
  for (int i = 0; i < 2; ++i)
    api::readFromCorrelator(
        handle_.get(),
        garbage->correlator_data.data(),
        reinterpret_cast<uint8_t*>(&garbage->footer),
        reinterpret_cast<uint8_t*>(&garbage->intensity_map));

  // Currently, the correlation_id is not reset between acquisitions because software reset of the
  // correlator is broken. PF is working on it. Once they fix it (and we remove the 2-frame-purge
  // above), correlation_id_offset should always start at 1.
  auto correlation_id_offset = garbage->footer.correlation_id + 1;
  g_reporter->debug("pf32_ll::AcquisitionG2: correlation_id_offset is {}", correlation_id_offset);

  garbage.reset();

  for (uint64_t seq_num = 0; !acquisition_stop_signal_.ShouldStop(); ++seq_num) {
    // Get some pre-allocated space to read data into
    storage_pool::FactoryHandle<RawCorrelatorResult> raw_result;
    try {
      raw_result = raw_results_storage_pool_.CreateForOverwrite();
    } catch (const std::bad_alloc&) {
      throw Exception("Software buffer overflow (Raw result conversion too slow)");
    }

    // Read a correlator result from the camera into raw_result
    auto ret = api::readFromCorrelator(
        handle_.get(),
        raw_result->correlator_data.data(),
        reinterpret_cast<uint8_t*>(&raw_result->footer),
        reinterpret_cast<uint8_t*>(&raw_result->intensity_map));
    if (!ret) {
      throw Exception("readFromCorrelator returned false. Is the correct firmware loaded?");
    }

    // Check for dropped frames
    auto expected_correlation_id = correlation_id_offset + seq_num;
    if (raw_result->footer.correlation_id != expected_correlation_id) {
      g_reporter->debug(
          "pf32_ll::AcquisitionG2: expected correlation_id {}, got {}",
          expected_correlation_id,
          raw_result->footer.correlation_id);
      throw Exception("Detected dropped frames (Acquisition too slow)");
    }

    // Note the frame_id of the first frame used in the first result
    if (seq_num == 0) {
      // The very first frame from the camera has a nonzero first_frame_id field
      first_first_frame_id_ = raw_result->footer.first_frame_id;
    }

    // Check magic numbers in footer
    if (raw_result->footer.fixed_pattern_1 != kFixedPattern1 ||
        raw_result->footer.fixed_pattern_2 != kFixedPattern2) {
      throw Exception("Footer has bad magic, is the correct bitfile loaded?");
    }

    auto f = process_worker_.Async(&AcquisitionG2::ProcessRawResult, this, std::move(raw_result));

    {
      std::unique_lock<std::mutex> l{result_futures_m_};
      result_futures_.push(std::move(f));
    }
    result_futures_cv_.notify_one();
  }
}

void AcquisitionG2::ConvertCorrelatorOutput(
    const uint8_t* raw,
    double* converted,
    std::size_t enabledHeight,
    [[maybe_unused]] std::size_t width) {
  // 2.0^-30 (see https://en.cppreference.com/w/cpp/language/floating_literal)
  static constexpr double kPrecision = 0x1p-30;

  // Having fixed width makes autovectorizer's job easier, but caller must pass correct value
  assert(width == kRowColCount);

  auto raw_32 = reinterpret_cast<const uint32_t*>(raw);

  for (std::size_t i = 0; i < enabledHeight; ++i) {
    for (std::size_t j = 0; j < kMaxNPoints; ++j) {
      for (std::size_t k = 0; k < kRowColCount; ++k) {
        *converted = (*raw_32) * kPrecision;
        converted++;
        raw_32++;
      }
    }
  }
}

storage_pool::FactoryHandle<AcquisitionG2::CorrelatorResult> AcquisitionG2::ProcessRawResult(
    storage_pool::FactoryHandle<RawCorrelatorResult> raw_result) {
  assert(raw_result.get());

  storage_pool::FactoryHandle<CorrelatorResult> result;
  try {
    result = results_storage_pool_.CreateForOverwrite();
  } catch (const std::bad_alloc&) {
    throw Exception("Software buffer overflow (Processing too slow)");
  }

  // Populate metadata fields
  result->begin_frame_idx = raw_result->footer.first_frame_id - first_first_frame_id_;
  result->frame_count = raw_result->footer.tint_n_frames;
  result->n_points = n_points_;

  // Extract marker channel events
  auto rising_edges = raw_result->footer.markers & (~markers_prev_);
  markers_prev_ = raw_result->footer.markers;
  for (size_t i = 0; i < kMarkerChannelCount; i++) {
    result->markers[i] = (rising_edges >> i) & 0x01;
  }

  // Convert fixed point data to doubles
  ConvertCorrelatorOutput(
      raw_result->correlator_data.data(), result->g2[0][0].data(), enabled_height_, kRowColCount);

  // Populate tau values
  int tau_nframes = 0;
  int tau_spacing = 1;
  auto next_rebin_idx_it = kRebinningIndices.cbegin();
  auto next_rebin_factor_it = rebin_factors_.cbegin();
  for (size_t k = 0; k < n_points_; ++k) {
    tau_nframes += tau_spacing;

    result->tau_k[k] =
        std::chrono::round<decltype(result->tau_k)::value_type>(frame_period_ps_ * tau_nframes);

    // Iteratively multiply tau_spacing at each rebinning point
    if (next_rebin_idx_it != kRebinningIndices.cend() && k == *next_rebin_idx_it) {
      tau_spacing *= *next_rebin_factor_it;
      ++next_rebin_idx_it;
      ++next_rebin_factor_it;
    }
  }

  // Copy photon counts
  result->intensity_map = raw_result->intensity_map;

  return result;
}

} // namespace pf32_ll
} // namespace pando
} // namespace pnd
