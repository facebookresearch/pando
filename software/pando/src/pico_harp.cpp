#include "pico_harp.h"

#include "ph300_driver.h"
#include "reporter.h"

#include <cassert>
#include <cmath>
#include <thread>

namespace pnd {
namespace pando {

void PicoHarpT2Base::ConsumeRecord(const Record& tag) {
  const auto tt = reinterpret_cast<const T2TimeTag*>(&tag);

  if (tt->channel != 0xF) {
    // Non-special record
    if (tt->channel > 4) {
      throw XHarpException("normal w/ channel " + std::to_string(tt->channel));
    }

    picoseconds macro_time{picosecondsPerTick_ * (overflow_state_ + tt->time)};
    // check continuity
    if (macro_time < last_timestamp_macro_time_) {
      throw XHarpException("PicoHarpT2Base::ConsumeRecord: encountered non-monotonic macro time");
    }

    last_channel_ = tt->channel;
    last_timestamp_macro_time_ = macro_time;
    stashed_timestamp_ = true;

  } else {
    // Special record
    uint8_t markers = tt->time & 0xF;
    if (markers == 0) {
      // This is an overflow event
      overflow_state_ += kT2Wraparound;
    } else {
      // This is a marker
      auto stripped_time = tt->time & 0xFFFFFF0;

      picoseconds macro_time{picosecondsPerTick_ * (overflow_state_ + stripped_time)};
      if (macro_time < last_marker_macro_time_) {
        throw XHarpException("PicoHarpT2Base::ConsumeRecord: encountered non-monotonic macro time");
      }

      last_channel_ =
          markers; // last_channel_ is a bitfield of channels that received marker records
      last_marker_macro_time_ = macro_time;
      stashed_markers_ = true;
    }
  }
}

void PicoHarpT3Base::ConsumeRecord(const Record& tag) {
  const auto tt = reinterpret_cast<const T3TimeTag*>(&tag);

  uint64_t syncs_seen = overflow_state_ + tt->nsync;

  if (tt->channel != 0xF) { // normal record
    if (tt->channel > 4) {
      throw XHarpException("normal w/ channel " + std::to_string(tt->channel));
    }

    auto channel = tt->channel;

    picoseconds micro_time{picosecondsPerTick_ * tt->dtime};
    picoseconds macro_time{syncs_seen * config_.laser_sync_period_ps};

    if (macro_time < last_timestamp_macro_time_) {
      throw XHarpException("PicoHarpT3Base::ConsumeRecord: encountered non-monotonic macro time");
    }

    last_channel_ = tt->channel;
    last_timestamp_macro_time_ = macro_time;
    last_micro_time_ = micro_time;
    stashed_timestamp_ = true;

  } else { // special record
    uint8_t markers = tt->dtime & 0xF;
    if (markers == 0) {
      // This is an overflow event
      overflow_state_ += kT3Wraparound;
    } else {
      // This is a marker
      auto stripped_time = tt->dtime & 0xFFFFFF0;

      picoseconds macro_time{syncs_seen * config_.laser_sync_period_ps};
      if (macro_time < last_marker_macro_time_) {
        throw XHarpException("PicoHarpT2Base::ConsumeRecord: encountered non-monotonic macro time");
      }

      last_channel_ =
          markers; // last_channel_ is a bitfield of channels that received marker records
      last_marker_macro_time_ = macro_time;
      stashed_markers_ = true;
    }
  }
}

void RealPicoHarpBase::Configure() {
  driver_->Calibrate();

  // Enable all marker channels.
  driver_->SetMarkerEnable(true, true, true, true);
  // Set all marker edges to rising edge.
  driver_->SetMarkerEdges(true, true, true, true);

  switch (config_.xharp_sync_div) {
    case 1:
    case 2:
    case 4:
    case 8:
      driver_->SetSyncDiv(config_.xharp_sync_div);
      break;
    default:
      throw XHarpException("xharp_sync_div must be a power of 2 not greater than 8");
  }

  // set resolution equal to BaseResolution
  driver_->SetBinning(0);

  // sync channel setup
  if (config_.xharp_sync_level > 800 || config_.xharp_sync_level < 0) {
    throw XHarpException("xharp_sync_level out of range [0, 800]");
  } else if (config_.xharp_sync_zerox > 20 || config_.xharp_sync_zerox < 0) {
    throw XHarpException("xharp_sync_zerox out of range [0, 20]");
  }

  if (config_.xharp_sync_offset > 99999 || config_.xharp_sync_offset < -99999) {
    throw XHarpException("xharp_sync_offset out of range [-99999, 99999]");
  }
  driver_->SetSyncOffset(config_.xharp_sync_offset);
  driver_->SetInputCFD(0, config_.xharp_sync_level, config_.xharp_sync_zerox);

  // input channel setup
  if (std::find(config_.enabled_channels.begin(), config_.enabled_channels.end(), 1) !=
      config_.enabled_channels.end()) {
    if (config_.xharp_level.find(1) != config_.xharp_level.end() &&
        config_.xharp_zerox.find(1) != config_.xharp_zerox.end()) {
      if (config_.xharp_level[1] > 800 || config_.xharp_level[1] < 0) {
        throw XHarpException(fmt::format("xharp_level out of range [0, 800] for channel {}", 1));
      } else if (config_.xharp_zerox[1] > 20 || config_.xharp_zerox[1] < 0) {
        throw XHarpException(fmt::format("xharp_zerox out of range [0, 20] for channel {}", 1));
      }

      driver_->SetInputCFD(1, config_.xharp_level[1], config_.xharp_zerox[1]);
    } else {
      // must set the CFD parameters for all enabled chanels, throw if this is not the case.
      throw XHarpException(fmt::format(
          "Channel {} is enabled, but the CFD parameters (xharp_zerox, xharp_level) for channel {} are not set",
          1,
          1));
    }
  }

  auto res_info = driver_->GetBaseResolution();
  picosecondsPerTick_ = static_cast<uint64_t>(res_info.resolution + 0.5);
  assert(std::abs(picosecondsPerTick_ - res_info.resolution) < 1E-6);
  g_reporter->info(
      "RealPicoHarpBase::Configure(): Set picosecondsPerTick_ to {}.", picosecondsPerTick_);
  g_reporter->info("RealHydraHarp::Configure(): GetResolution() = {}", driver_->GetResolution());
  // After Init or SetSyncDiv you must allow >100 ms for valid count rate readings
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  for (int channel = 0; channel < 2; ++channel) {
    auto rate = driver_->GetCountRate(channel);
    if (channel == 0 && config_.device == PandoInterface::DeviceType::PICO_HARP_T3) {
      // do a sanity check on laser sync period
      int calculated_sync_period_ps = static_cast<int>((1.0 / static_cast<double>(rate)) * 1.0E+12);
      g_reporter->debug("Calculated laser sync period is {:n}", calculated_sync_period_ps);
      CheckLaserSyncPeriod(calculated_sync_period_ps);
    }
    g_reporter->debug("Pico channel {} count rate is {}", channel, rate);
  }
}

void RealPicoHarpBase::Acquire() {
  RecordBuffer* read_buff = ring_buffer_.AdvanceWrite();
  read_buff->occupancy = 0;

  // Start a 100 hour long acquisition (we will stop it early.)
  driver_->StartMeas(ACQTMAX);

  // Read data from the device FIFO
  while (!acquisition_stop_signal_.ShouldStop()) {
    auto flags = driver_->GetFlags();

    if (flags & FLAG_SYSERROR) {
      g_reporter->warn(
          "FLAG_SYSERROR is set, PH_GetHardwareDebugInfo output:\n {}",
          driver_->GetHardwareDebugInfo());
      throw XHarpException("FLAG_SYSERROR was set");
    }

    if (flags & FLAG_FIFOFULL) {
      driver_->StopMeas();
      throw XHarpException("Device FIFO overflowed");
    }

    // Flip the buffer if possible
    if (auto new_buffer = ring_buffer_.MaybeAdvanceWrite()) {
      new_buffer->occupancy = 0;
      read_buff = new_buffer;
    }

    // Check if we have space to read TTREADMAX records from the FIFO
    if (read_buff->occupancy + TTREADMAX > kReadBufferSize) {
      throw XHarpException("Software FIFO overflowed");
    }

    auto n_records = driver_->ReadFifo(read_buff->data + read_buff->occupancy, TTREADMAX);
    read_buff->occupancy += n_records;

    if (n_records == 0) {
      // If the FIFO is empty, check if the device thinks the acquisition has ended
      if (driver_->CTCStatus()) {
        break;
      }

      // Wait for the FIFO to fill up a bit
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  }

  driver_->StopMeas();
}

PicoHarpT2::PicoHarpT2() {
  driver_ = Driver::OpenFirstDevice();
  driver_->Initialize(MODE_T2);

  g_reporter->info(
      "PicoHarpT2: Opened device {} with SN {}", driver_->dev_idx_, driver_->serial_number_);
}

PicoHarpT3::PicoHarpT3() {
  driver_ = Driver::OpenFirstDevice();
  driver_->Initialize(MODE_T3);

  g_reporter->info(
      "PicoHarpT3: Opened device {} with SN {}", driver_->dev_idx_, driver_->serial_number_);
}

} // namespace pando
} // namespace pnd
