#include "multi_harp.h"

#include "multi_harp_driver.h"
#include "reporter.h"

#include <cassert>
#include <cmath>
#include <thread>

namespace pnd {
namespace pando {

void MultiHarpT2Base::ConsumeRecord(Record tag) {
  const auto tt = reinterpret_cast<const T2TimeTag*>(&tag);

  picoseconds macro_time;

  if (!tt->special) {
    if (tt->channel >= 16) {
      throw XHarpException("normal w/  channel " + std::to_string(tt->channel));
    }

    macro_time = picoseconds{(overflow_state_ + tt->timetag) * picosecondsPerTick_};

    // check continuity
    if (macro_time < last_timestamp_macro_time_) {
      throw XHarpException("MultiHarpT2Base::ConsumeRecord: encountered non-monotonic macro time");
    }

    last_channel_ = tt->channel;
    last_timestamp_macro_time_ = macro_time;
    stashed_timestamp_ = true;

  } else {
    switch (tt->channel) {
      case 0:
        // This is a SYNC event
        // sync_count++;
        break;
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
        macro_time = picoseconds{(overflow_state_ + tt->timetag) * picosecondsPerTick_};

        // check continuity
        if (macro_time < last_marker_macro_time_) {
          throw XHarpException(
              "HydraHarpT2Base::ConsumeRecord: encountered non-monotonic macro time");
        }

        last_channel_ = tt->channel;
        last_marker_macro_time_ = macro_time;
        stashed_markers_ = true;
        break;
      case 63:
        // This is an overflow event
        // overflow_count++;
        overflow_state_ += kT2Wraparound * tt->timetag;
        break;
      default:
        throw XHarpException("special w/ unknown channel");
        break;
    }
  }
}

void MultiHarpT3Base::ConsumeRecord(Record tag) {
  const auto tt = reinterpret_cast<const T3TimeTag*>(&tag);

  uint64_t syncs_seen = overflow_state_ + tt->nsync;

  picoseconds macro_time;

  if (!tt->special) {
    if (tt->channel >= 16) {
      throw XHarpException("normal w/  channel " + std::to_string(tt->channel));
    }

    picoseconds micro_time{picosecondsPerTick_ * tt->dtime};
    macro_time = picoseconds{syncs_seen * config_.laser_sync_period_ps};

    if (macro_time < last_timestamp_macro_time_) {
      throw XHarpException("MultiHarpT3Base::ConsumeRecord: encountered non-monotonic macro time");
    }

    last_channel_ =
        tt->channel; // last_channel_ is a bitfield of channels that received marker records
    last_timestamp_macro_time_ = macro_time;
    last_micro_time_ = micro_time;
    stashed_timestamp_ = true;

  } else {
    switch (tt->channel) {
      case 63:
        // overflow event
        overflow_state_ += kT3Wraparound * tt->nsync; // for HH > V1, nsync records
        break; // the number of overflows
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
        macro_time = picoseconds{syncs_seen * config_.laser_sync_period_ps};
        // check continuity
        if (macro_time < last_marker_macro_time_) {
          throw XHarpException(
              "HydraHarpT3Base::ConsumeRecord: encountered non-monotonic macro time");
        }

        last_channel_ =
            tt->channel; // last_channel_ is a bitfield of channels that received marker records
        last_marker_macro_time_ = macro_time;
        stashed_markers_ = true;
        break;
      default:
        throw XHarpException("special w/ unknown channel");
        break;
    }
  }
}

void RealMultiHarpBase::Configure() {
  switch (config_.xharp_sync_div) {
    case 1:
    case 2:
    case 4:
    case 8:
    case 16:
      driver_->SetSyncDiv(config_.xharp_sync_div);
      break;
    default:
      throw XHarpException("xharp_sync_div must be a power of 2 not greater than 16");
  }

  // set resolution equal to BaseResolution
  driver_->SetBinning(0);

  if (config_.xharp_sync_offset > 99999 || config_.xharp_sync_offset < -99999) {
    throw XHarpException("xharp_sync_offset out of range [-99999, 99999]");
  } else if (
      config_.multi_harp_sync_trigger_edge != 0 && config_.multi_harp_sync_trigger_edge != 1) {
    throw XHarpException("multi_harp_sync_trigger_edge not 0 or 1");
  } else if (
      config_.multi_harp_sync_trigger_level_mv > 1200 ||
      config_.multi_harp_sync_trigger_level_mv < -1200) {
    throw XHarpException("multi_harp_sync_trigger_level_mv out of range [-1200, 1200]");
  } else if (config_.multi_harp_sync_tdead_ps < 0 || config_.multi_harp_sync_tdead_ps > 160000) {
    throw XHarpException("multi_harp_sync_tdead_ns out of range [0, 160]");
  }

  // set HW values from config
  driver_->SetSyncEdgeTrg(
      config_.multi_harp_sync_trigger_level_mv, config_.multi_harp_sync_trigger_edge);
  int dead_time_on = (config_.multi_harp_sync_tdead_ps > 800) ? 1 : 0;
  auto sync_dead_time =
      (config_.multi_harp_sync_tdead_ps > 800) ? config_.multi_harp_sync_tdead_ps : 800;
  driver_->SetSyncDeadTime(dead_time_on, sync_dead_time);
  driver_->SetSyncChannelOffset(config_.xharp_sync_offset);

  auto res_info = driver_->GetBaseResolution();
  picosecondsPerTick_ = static_cast<uint64_t>(res_info.resolution + 0.5);
  assert(std::abs(picosecondsPerTick_ - res_info.resolution) < 1E-6);
  g_reporter->info(
      "RealMultiHarpBase::Configure(): Set picosecondsPerTick_ to {}.", picosecondsPerTick_);
  g_reporter->info(
      "RealMultiHarpBase::Configure(): GetResolution() = {}", driver_->GetResolution());

  // Note that the channel parameters in the config struct are 1-indexed (1-16)
  // But the channel setters for MultiHarp are 0-indexed (0-15)
  auto n_channels = driver_->GetNumOfInputChannels();
  for (auto i = 0; i < n_channels; i++) {
    // don't need to set the values unless we're using the channel
    // so check if it's present in enabled_channels
    if (std::find(config_.enabled_channels.begin(), config_.enabled_channels.end(), i) !=
        config_.enabled_channels.end()) {
      if (config_.multi_harp_trigger_level_mv.find(i) !=
              config_.multi_harp_trigger_level_mv.end() &&
          config_.multi_harp_trigger_edge.find(i) != config_.multi_harp_trigger_edge.end()) {
        // check the range of the settings
        if (config_.multi_harp_trigger_level_mv[i] > 1200 ||
            config_.multi_harp_trigger_level_mv[i] < -1200) {
          throw XHarpException(fmt::format(
              "multi_harp_trigger_level_mv out of range [-1200, 1200] for channel {}", i));
        } else if (
            config_.multi_harp_trigger_edge[i] != 0 && config_.multi_harp_trigger_edge[i] != 1) {
          throw XHarpException(
              fmt::format("multi_harp_trigger_edge out of range [0, 1] for channel {}", i));
        }

        driver_->SetInputEdgeTrg(
            i, config_.multi_harp_trigger_level_mv[i], config_.multi_harp_trigger_edge[i]);
      } else {
        // must set the CFD parameters for all enabled chanels, throw if this is not the case.
        throw XHarpException(fmt::format(
            "Channel {} is enabled, but the trigger parameters (multi_harp_trigger_edge, multi_harp_trigger_level_mv) for channel {} are not set",
            i,
            i));
      }

      // set the channel offset, default 0
      int channel_offset = 0;
      if (config_.hharp_input_offsets.find(i) != config_.hharp_input_offsets.end()) {
        channel_offset = config_.hharp_input_offsets[i];
        if (channel_offset > 99999 || channel_offset < -99999) {
          throw XHarpException(
              fmt::format("hharp_input_offset for channel {} out of range [-99999, 99999]", i));
        }
      }
      driver_->SetInputChannelOffset(i, channel_offset);

      // set the dead time, default is 0 (off)
      int t_dead = 0;
      int t_dead_on = 0;
      if (config_.multi_harp_tdead_ps.find(i) != config_.multi_harp_tdead_ps.end()) {
        t_dead = config_.multi_harp_tdead_ps[i];
        if (t_dead >= 800) {
          t_dead_on = 1;
        } else {
          g_reporter->debug(
              "RealMultiHarpBase::Configure(): Programmable dead time for channel {} is {} which is less than 800 so turning off dead time for this channel.",
              channel_offset,
              t_dead);
          t_dead = 800; // even though we're turning dead time off,
                        // can't pass less than 800 to lib call
        }
        if (t_dead > 160000) {
          throw XHarpException(
              fmt::format("multi_harp_tdead_ps for channel {} out of range [800, 160000]", i));
        }
        driver_->SetInputDeadTime(i, t_dead_on, t_dead);
      }
    }
  }

  // After Init or SetSyncDiv you must allow >100 ms for valid count rate readings
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  if (config_.device == PandoInterface::DeviceType::MULTI_HARP_T3) {
    // do a sanity check on laser sync period
    auto sync_rate = driver_->GetSyncRate();
    long calculated_sync_period_ps =
        static_cast<long>((1.0 / static_cast<double>(sync_rate)) * 1.0E+12);
    g_reporter->debug("Calculated laser sync period is {:n}", calculated_sync_period_ps);
    CheckLaserSyncPeriod(calculated_sync_period_ps);
  }
}

void RealMultiHarpBase::LogStartStopData() {
  g_reporter->debug("MultiHarp sync rate is {:n}", driver_->GetSyncRate());
  int total_count_rate = 0;
  auto n_channels = driver_->GetNumOfInputChannels();
  for (auto channel = 0; channel < n_channels; ++channel) {
    int count_rate = driver_->GetCountRate(channel);
    g_reporter->debug("MultiHarp channel {} count rate is {:n}", channel, count_rate);
    total_count_rate += count_rate;
  }
  g_reporter->debug("Total count rate is {:n}", total_count_rate);
}

void RealMultiHarpBase::Acquire() {
  RecordBuffer* read_buff = ring_buffer_.AdvanceWrite();
  read_buff->occupancy = 0;

  // Start a 100 hour long acquisition (we will stop it early.)
  driver_->StartMeas(ACQTMAX);

  // Read data from the device FIFO
  while (!acquisition_stop_signal_.ShouldStop()) {
    // Check if FIFO has overflowed
    auto flags = driver_->GetFlags();
    // if (flags & ~FLAG_ACTIVE) {
    //   std::cout << "driver_->GetFlags() returned " << flags << std::endl;
    // }

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
    auto n_records = driver_->ReadFifo(read_buff->data + read_buff->occupancy);
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

MultiHarpT2::MultiHarpT2() {
  driver_ = Driver::OpenFirstDevice();
  driver_->Initialize(MODE_T2);

  g_reporter->info(
      "MultiHarpT2: Opened device {} with SN {}", driver_->dev_idx_, driver_->serial_number_);
}

MultiHarpT3::MultiHarpT3() {
  driver_ = Driver::OpenFirstDevice();
  driver_->Initialize(MODE_T3);

  g_reporter->info(
      "MultiHarpT3: Opened device {} with SN {}", driver_->dev_idx_, driver_->serial_number_);
}

} // namespace pando
} // namespace pnd
