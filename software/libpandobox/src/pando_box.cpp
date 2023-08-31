#include <algorithm>
#include <cstring>

#include "pando_box.h"

#include <iostream>
#include <stdexcept>

namespace pnd {
namespace libpandobox {

PandoBox::PandoBox() : dma_{AxiDma(8)}, dma_buffer_{dma_.GetBufferPointer()} {
  SetRun(false);

  // Disable all peripherals
  SetDin0Enabled(false);
  SetImu0Enabled(false);
  SetAin0Enabled(false);
  SetAin1Enabled(false);
  SetAin2Enabled(false);
  SetAin3Enabled(false);
  SetTrgen0Enabled(false);

  // Put all peripherals in non-mocked mode
  SetDin0Mocked(false);
  SetImu0Mocked(false);
  SetAin0Mocked(false);
  SetAin1Mocked(false);
  SetAin2Mocked(false);
  SetAin3Mocked(false);

  // Set all sample rates to 1kHz
  SetAin0SampRateDiv(10000);
  SetAin1SampRateDiv(10000);
  SetAin2SampRateDiv(10000);
  SetAin3SampRateDiv(10000);
  SetTrgen0SampRateDiv(10000);
}

void PandoBox::InitDma() {
  // Initialize DMA system
  dma_.Reset();

  // We don't want an interrupt for every packet, by changing
  // kPacketsPerInterrupt, the interrupt frequency can be adjusted
  dma_.SetInterruptThreshold(kPacketsPerInterrupt, kS2mm);
  dma_.UnmaskInterrupt();

  // Build out our descriptor chain.  This is a circular linked list,
  // with each descriptor being ultimately responsible for the
  // transmission of a single sample into memory.
  current_desc_idx_ = 0;
  for (unsigned i = 0; i < dma_.kMaxDescriptors; i++) {
    // Make sure there isn't any garbage in the descriptor
    dma_.ResetDescriptor(i);

    dma_.SetDescriptorBufferOffset(i, kMaxPacketSize * i);
    dma_.SetDescriptorTxLen(i, kMaxPacketSize);

    dma_.SetDescriptorNext(i, (i + 1) % dma_.kMaxDescriptors);
  }
}

void PandoBox::StartDma() {
  // Starts a DMA transaction on the S2MM (Stream to Memory) channel
  // with interrupts and cyclic operation enabled
  InitDma();
  dma_.ExecuteDescriptorChain(0, kS2mm, true, true);
}

void PandoBox::StopDma() {
  dma_.Stop();
}

int PandoBox::Read(char* dest, unsigned int len, int timeout) {
  dma_.WaitForInterrupt(timeout);

  if (dma_.GetStatus(kS2mm).sg_internal_error) {
    throw std::runtime_error("DMA overflowed (SG engine reached tail descriptor)");
  }

  unsigned int remaining = len;
  // Try to consume as much data as possible
  while (remaining) {
    auto stat = dma_.GetDescriptorStatus(current_desc_idx_);

    // Don't try to transfer partial packets
    if (!stat.completed) {
      break;
    }

    // Don't try to transfer if we can't fit it in the buffer
    if (stat.bytes_transferred > remaining) {
      break;
    }

    // Copy our data in using the DMA memcpy routine to avoid bus errors
    auto desc_data = dma_.GetDescriptorData(current_desc_idx_);
    dest = dma_.MemCpy(dest, desc_data.data(), stat.bytes_transferred);
    remaining -= stat.bytes_transferred;

    // Housekeeping on our descriptors.  This clears the completed and
    // amount transferred information.
    dma_.ResetDescriptorStatus(current_desc_idx_);
    current_desc_idx_ = dma_.GetDescriptorNext(current_desc_idx_);
  }

  return len - remaining;
}

std::size_t PandoBox::ConsumeSamples(
    std::function<void(const std::vector<const sample_format::PandoBox*>& samples)> consume,
    unsigned int max_samples,
    int timeout) {
  dma_.WaitForInterrupt(timeout);

  if (dma_.GetStatus(kS2mm).sg_internal_error) {
    throw std::runtime_error("DMA overflowed (SG engine reached tail descriptor)");
  }

  std::vector<const sample_format::PandoBox*> samples;
  std::vector<decltype(current_desc_idx_)> descriptors_to_reset;

  samples.reserve(max_samples);
  descriptors_to_reset.reserve(max_samples);

  // Prepare to consume data from up to max_samples completed descriptors
  for (auto sample_count = 0; sample_count < max_samples; sample_count++) {
    auto stat = dma_.GetDescriptorStatus(current_desc_idx_);

    // Don't try to transfer partial packets
    if (!stat.completed) {
      break;
    }

    auto desc_data = dma_.GetDescriptorData(current_desc_idx_);
    auto sample = reinterpret_cast<const sample_format::PandoBox*>(desc_data.data());
    samples.push_back(sample);
    descriptors_to_reset.push_back(current_desc_idx_);

    current_desc_idx_ = dma_.GetDescriptorNext(current_desc_idx_);
  }

  // Consume the data referenced by the descriptors
  consume(samples);

  // Reset the descriptors whose data we consumed
  for (auto desc_idx : descriptors_to_reset) {
    dma_.ResetDescriptorStatus(desc_idx);
  }

  return samples.size();
}

PandoBox::~PandoBox() {
  SetRun(false);
}

void PandoBox::SetRun(bool run) {
  global_config_regs_->flags_0.WriteRun(run);
}

uint32_t PandoBox::GetVersion() {
  return global_config_regs_->version;
};

void PandoBox::SetDin0Enabled(bool enabled) {
  din0_config_regs_->flags_0.WriteEnable(enabled);
}

void PandoBox::SetImu0Enabled(bool enabled) {
  imu0_config_regs_->flags_0.WriteEnable(enabled);
}

void PandoBox::SetAin0Enabled(bool enabled) {
  ain0_config_regs_->flags_0.WriteEnable(enabled);
}

void PandoBox::SetAin1Enabled(bool enabled) {
  ain1_config_regs_->flags_0.WriteEnable(enabled);
}

void PandoBox::SetAin2Enabled(bool enabled) {
  ain2_config_regs_->flags_0.WriteEnable(enabled);
}

void PandoBox::SetAin3Enabled(bool enabled) {
  ain3_config_regs_->flags_0.WriteEnable(enabled);
}

void PandoBox::SetTrgen0Enabled(bool enabled) {
  trgen0_config_regs_->flags_0.WriteEnable(enabled);
}

void PandoBox::SetTrigGen0Enabled(bool enabled) {
  triggergen0_config_regs_->flags_0.WriteEnable(enabled);
}

void PandoBox::SetTrigGen1Enabled(bool enabled) {
  triggergen1_config_regs_->flags_0.WriteEnable(enabled);
}

void PandoBox::SetDin0Mocked(bool mock) {
  din0_config_regs_->flags_0.WriteMock(mock);
}

void PandoBox::SetImu0Mocked(bool mock) {
  imu0_config_regs_->flags_0.WriteMock(mock);
}

void PandoBox::SetAin0Mocked(bool mock) {
  ain0_config_regs_->flags_0.WriteMock(mock);
}

void PandoBox::SetAin1Mocked(bool mock) {
  ain1_config_regs_->flags_0.WriteMock(mock);
}

void PandoBox::SetAin2Mocked(bool mock) {
  ain2_config_regs_->flags_0.WriteMock(mock);
}

void PandoBox::SetAin3Mocked(bool mock) {
  ain3_config_regs_->flags_0.WriteMock(mock);
}

void PandoBox::SetAin0SampRateDiv(uint32_t divisor) {
  ain0_config_regs_->sample_rate_div = divisor;
}

void PandoBox::SetAin1SampRateDiv(uint32_t divisor) {
  ain1_config_regs_->sample_rate_div = divisor;
}

void PandoBox::SetAin2SampRateDiv(uint32_t divisor) {
  ain2_config_regs_->sample_rate_div = divisor;
}

void PandoBox::SetAin3SampRateDiv(uint32_t divisor) {
  ain3_config_regs_->sample_rate_div = divisor;
}

void PandoBox::SetTrgen0SampRateDiv(uint32_t divisor) {
  trgen0_config_regs_->sample_rate_div = divisor;
}

void PandoBox::SetTrigGen0Period(uint32_t trigger_period) {
  triggergen0_config_regs_->trigger_period = trigger_period;
}

void PandoBox::SetTrigGen1Period(uint32_t trigger_period) {
  triggergen1_config_regs_->trigger_period = trigger_period;
}

void PandoBox::SetTrigGen0Width(uint32_t trigger_width) {
  triggergen0_config_regs_->trigger_width = trigger_width;
}

void PandoBox::SetTrigGen1Width(uint32_t trigger_width) {
  triggergen1_config_regs_->trigger_width = trigger_width;
}

} // namespace libpandobox
} // namespace pnd
