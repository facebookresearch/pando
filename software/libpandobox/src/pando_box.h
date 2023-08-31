#pragma once

#include "dma.h"
#include "pando_box_interface.h"

#include "peripheral_registers.h"
#include "uio_device.h"

namespace pnd {
namespace libpandobox {

class PandoBox : public PandoBoxInterface {
 public:
  static constexpr int kMaxPacketSize = sizeof(sample_format::PandoBox);
  static constexpr int kPacketsPerInterrupt = 32;
  PandoBox();
  ~PandoBox();

 private:
  void SetRun(bool run) final;
  uint32_t GetVersion() final;

  void SetDin0Enabled(bool enabled) final;
  void SetImu0Enabled(bool enabled) final;
  void SetAin0Enabled(bool enabled) final;
  void SetAin1Enabled(bool enabled) final;
  void SetAin2Enabled(bool enabled) final;
  void SetAin3Enabled(bool enabled) final;
  void SetTrgen0Enabled(bool enabled) final;
  void SetTrigGen0Enabled(bool enabled) final;
  void SetTrigGen1Enabled(bool enabled) final;

  void SetDin0Mocked(bool mock) final;
  void SetImu0Mocked(bool mock) final;
  void SetAin0Mocked(bool mock) final;
  void SetAin1Mocked(bool mock) final;
  void SetAin2Mocked(bool mock) final;
  void SetAin3Mocked(bool mock) final;

  void SetAin0SampRateDiv(uint32_t divisor) final;
  void SetAin1SampRateDiv(uint32_t divisor) final;
  void SetAin2SampRateDiv(uint32_t divisor) final;
  void SetAin3SampRateDiv(uint32_t divisor) final;

  void SetTrgen0SampRateDiv(uint32_t divisor) final;

  void SetTrigGen0Period(uint32_t trigger_period) final;
  void SetTrigGen1Period(uint32_t trigger_period) final;

  void SetTrigGen0Width(uint32_t trigger_width) final;
  void SetTrigGen1Width(uint32_t trigger_width) final;

  UioRegisters<reg::Global> global_config_regs_{reg::Global::kUioDevName};
  UioRegisters<reg::DigitalInput> din0_config_regs_{reg::DigitalInput::kUioDevNameInst0};
  UioRegisters<reg::Imu> imu0_config_regs_{reg::Imu::kUioDevNameInst0};
  UioRegisters<reg::AnalogInput> ain0_config_regs_{reg::AnalogInput::kUioDevNameInst0};
  UioRegisters<reg::AnalogInput> ain1_config_regs_{reg::AnalogInput::kUioDevNameInst1};
  UioRegisters<reg::AnalogInput> ain2_config_regs_{reg::AnalogInput::kUioDevNameInst2};
  UioRegisters<reg::AnalogInput> ain3_config_regs_{reg::AnalogInput::kUioDevNameInst3};
  UioRegisters<reg::TrafficGenerator> trgen0_config_regs_{reg::TrafficGenerator::kUioDevNameInst0};
  UioRegisters<reg::TriggerGen> triggergen0_config_regs_{reg::TriggerGen::kUioDevNameInst0};
  UioRegisters<reg::TriggerGen> triggergen1_config_regs_{reg::TriggerGen::kUioDevNameInst1};

  void InitDma() final;
  void StartDma() final;
  void StopDma() final;

  int Read(char* dest, unsigned int len, int timeout) final;
  std::size_t ConsumeSamples(
      std::function<void(const std::vector<const sample_format::PandoBox*>& samples)> consume,
      unsigned int max_samples,
      int timeout) final;

  AxiDma dma_;
  std::size_t current_desc_idx_;
  AxiDmaBuffer dma_buffer_;
};

} // namespace libpandobox
} // namespace pnd
