#pragma once

#include <array>
#include <cstddef>
#include <string_view>

#include "mapped_ptr.h"
#include "uio_device.h"

namespace pnd {
namespace libpandobox {

// Registers for configuring the DMA peripheral
struct AxiDmaRegisters {
  volatile uint32_t DMACR;
  volatile const uint32_t DMASR;
  volatile uint32_t CURDESC;
  volatile uint32_t CURDES_MSB;
  volatile uint32_t TAILDESC;
  volatile uint32_t TAILDESC_MSB;
  volatile uint32_t DA_SA;
  volatile uint32_t DA_SA_MSB;
  const uint32_t _pad[2];
  volatile uint32_t LENGTH;
  volatile uint32_t SG_CTL;
} __attribute__((__packed__));

struct AxiDmaController {
  struct AxiDmaRegisters mm2s;
  struct AxiDmaRegisters s2mm;
} __attribute((__packed__));

struct AxiDmaDescriptorStatus {
  bool completed;
  bool decode_error;
  bool slave_error;
  bool internal_error;
  uint32_t transaction_len;
  uint32_t bytes_transferred;
};

enum AxiDmaChannel { kMm2s, kS2mm };

struct AxiDmaBuffer {
  const uint32_t offset;
  char* data;
};

struct AxiDmaStatus {
  bool halted;
  bool idle;
  bool sg_included;
  bool dma_internal_error;
  bool dma_slave_error;
  bool dma_decode_error;
  bool sg_internal_error;
  bool sg_slave_error;
  bool sg_decode_error;
  bool complete_irq;
  bool delay_irq;
  bool error_irq;
  uint8_t irq_threshold_status;
  uint8_t irq_delay_status;
};

/** Userspace AXI DMA via UIO class
 *
 * The AxiDma class provides a low-level abstraction over an AXI DMA
 * controller exposed to userspace via the linux UIO subsystem.
 * Interrupt control is handled via the underlying UioDevice base
 * class.
 *
 * Two major assumptions are made in this driver/class:
 * 1) There is a reserved memory region at physical location
 *    0x10000000.
 * 2) That reserved memory location is 0x100000 bytes long.
 *
 */
class AxiDma : public UioDevice {
 public:
  static constexpr uint32_t kMaxDescriptors = 4096;

  AxiDma(unsigned int uio_number);
  ~AxiDma();

  /** Forces the DMA block to reset, ending all existing transactions gracefully
   */
  void Reset();

  /** Clears the run flag, causing the DMA to end gracefully
   */
  void Stop();

  /* Resets a descriptor's control register and its pointer to the next descriptor.
   * @param descriptor_idx The index of the descriptor to operate on
   */
  void ResetDescriptor(std::size_t descriptor_idx);

  /* Resets a descriptor's status register, which zeros out the amount transferred and completed
   * flags.
   * @param descriptor_idx The index of the descriptor to operate on
   */
  void ResetDescriptorStatus(std::size_t descriptor_idx);

  /* Returns a snapshot of the current descriptor status
   * @param descriptor_idx The index of the descriptor to operate on
   */
  AxiDmaDescriptorStatus GetDescriptorStatus(std::size_t descriptor_idx) const;

  /** Print a descriptor's entire status to stdout.
   * @param descriptor_idx The index of the descriptor to operate on
   */
  void PrintDescriptorStatus(std::size_t descriptor_idx) const;

  /* Sets the offset of a descriptor's associated buffer in the buffer region of the reserved
   * memory.
   * @param descriptor_idx The index of the descriptor to operate on
   * @param length The length of the DMA transaction in bytes
   */
  void SetDescriptorBufferOffset(std::size_t descriptor_idx, std::size_t offset);

  /* Sets the total length of a descriptor's DMA transaction.
   * @param descriptor_idx The index of the descriptor to operate on
   * @param length The length of the DMA transaction in bytes
   */
  void SetDescriptorTxLen(std::size_t descriptor_idx, uint32_t length);

  /* Links two descriptors together in a linked-list configuration.
   * @param descriptor_idx The index of the descriptor to operate on
   * @param next_descriptor_idx The index of the descriptor to set as "next"
   */
  void SetDescriptorNext(std::size_t descriptor_idx, std::size_t next_descriptor_idx);

  /* Get the index of the next descriptor in the descriptor chain.
   * @param descriptor_idx The index of the descriptor to operate on
   */
  std::size_t GetDescriptorNext(std::size_t descriptor_idx) const;

  /* Get a reference to the data stored in a descriptor's associated buffer.
   * @param descriptor_idx The index of the descriptor to operate on
   */
  std::string_view GetDescriptorData(std::size_t descriptor_idx) const;

  /** Begins a DMA Descriptor transaction on a specified channel
   *
   * @param chain_start The head of a linked list of AxiDmaDescriptors
   * @param chan The channel (S2MM or MM2S) on which this transaction will occur
   * @param enable_interrupts If true, interrupts will be enabled and accessible via
   * `WaitForInterrupt`
   * @param cyclic_operation If true, the dma will run in cyclic mode, allowing for free-running
   * data streaming
   */
  void ExecuteDescriptorChain(
      std::size_t chain_start_idx,
      AxiDmaChannel chan,
      bool enable_interrupts,
      bool cyclic_operation);

  /** Returns a struct describing the physical memory address of our
   * reserved buffer and a char* allowing the user to access said memory
   */
  AxiDmaBuffer GetBufferPointer() const;

  /** Sets the number of interrupts that will be received by the DMA
   * hardware before an interrupt will be sent to the processor
   */
  void SetInterruptThreshold(uint8_t thresh, AxiDmaChannel chan);

  /** Returns a struct containing a snapshot of the status of the
   * dma controller for the specified channel
   */
  AxiDmaStatus GetStatus(AxiDmaChannel) const;
  void PrintStatus(AxiDmaChannel) const;

  /** memcpy, but pinned to 32 bit aligned transfers to
   * avoid bus errors
   */
  static char* MemCpy(char* dst, const char* src, size_t len);

 private:
  // DMA transaction descriptor structure.  In Scatter/Gather mode,
  // these structures are used to configure a chain of DMA transactions
  // to be taken in succession
  struct AxiDmaDescriptorRegisters {
    volatile uint32_t NXTDESC;
    volatile uint32_t NXTDESC_MSB;
    volatile uint32_t BUFFER_ADDRESS;
    volatile uint32_t BUFFER_ADDRESS_MSB;
    const uint32_t _reserved[2];
    volatile uint32_t CONTROL;
    volatile uint32_t STATUS;
    volatile uint32_t APP0;
    volatile uint32_t APP1;
    volatile uint32_t APP2;
    volatile uint32_t APP3;
    volatile uint32_t APP4;
  } __attribute__((__packed__));

  struct AxiDmaDescriptor {
    char* virtual_buffer; /**< Pointer to the virtual buffer associated with this descriptor. */
    uint32_t raw_address; /**< Physical address of regs. */
    AxiDmaDescriptor* next; /** Pointer to the next descriptor in a descriptor chain. */
    AxiDmaDescriptorRegisters* regs; /**< Hardware interface struct. */
  };

  static constexpr uint32_t kDescriptorSize = 0x40;
  static constexpr uint32_t kMemBase = 0x10000000;
  static constexpr uint32_t kMemLen = 0x100000;
  static constexpr uint32_t kBufferOffset = kDescriptorSize * kMaxDescriptors;
  static constexpr uint32_t kBufferStart = kMemBase + kBufferOffset;
  static constexpr uint32_t kBufferSize = kMemBase + kMemLen - kBufferStart;

  MappedPtr<AxiDmaController> registers_;

  int dev_mem_fd_;
  MappedPtr<std::array<char, kMemLen>> reserved_;
  std::array<AxiDmaDescriptor, kMaxDescriptors> descriptors_;
  char* buffer_;

  // Bit position definitions

  // DMA Control Block
  static constexpr int DMACR_SR = 0;
  static constexpr int DMACR_RESET = 2;
  static constexpr int DMACR_CyclicBDEn = 4;
  static constexpr int DMACR_ERRIrqEn = 14;
  static constexpr int DMACR_IOCIrqEn = 12;
  static constexpr int DMACR_IRQThresh = 16;
  static constexpr int DMACR_IRQDelay = 24;

  // DMA Status Block
  static constexpr int DMASR_Halted = 0;
  static constexpr int DMASR_Idle = 1;
  static constexpr int DMASR_SGIncld = 3;
  static constexpr int DMASR_IntErr = 4;
  static constexpr int DMASR_SlvErr = 5;
  static constexpr int DMASR_DecErr = 6;
  static constexpr int DMASR_SGIntErr = 8;
  static constexpr int DMASR_SGSlvErr = 9;
  static constexpr int DMASR_SGDecErr = 10;
  static constexpr int DMASR_IOCIrq = 12;
  static constexpr int DMASR_DlyIrq = 13;
  static constexpr int DMASR_ErrIrq = 14;
  static constexpr int DMASR_IrqThreshSts = 16;
  static constexpr int DMASR_IrqDelaySts = 24;

  // Descriptor Control Block
  static constexpr int DESCCR_BufLenMask = ((1 << 26) - 1);

  // Descriptor Status Block
  static constexpr int DESCSR_TransferredBytesMask = ((1 << 26) - 1);

  static constexpr int DESCSR_Cmplt = 31;
  static constexpr int DESCSR_DMADecErr = 30;
  static constexpr int DESCSR_DMASlvErr = 29;
  static constexpr int DESCSR_DMAIntErr = 28;
};

} // namespace libpandobox
} // namespace pnd
