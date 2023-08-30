#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "dma.h"
#include "uio_device.h"

namespace pnd {
namespace libpandobox {

AxiDma::AxiDma(unsigned int uio_number)
    : UioDevice{uio_number}, registers_{PROT_READ | PROT_WRITE, MAP_SHARED, uio_fd_, 0} {
  dev_mem_fd_ = open("/dev/mem", O_RDWR | O_SYNC);

  reserved_ = {PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, dev_mem_fd_, kMemBase};

  // raw pointer to the beginning of the reserved memory region, for pointer arithmetic
  auto ptr = reserved_.get()->data();

  buffer_ = ptr + kBufferOffset;

  // Create a pool of all descriptors
  for (unsigned i = 0; i < kMaxDescriptors; i++) {
    auto offset = (i * kDescriptorSize);

    descriptors_[i].regs = reinterpret_cast<AxiDmaDescriptorRegisters*>(ptr + offset);
    descriptors_[i].raw_address = kMemBase + offset;
  }
}

AxiDmaBuffer AxiDma::GetBufferPointer() const {
  return AxiDmaBuffer{kBufferStart, buffer_};
}

void AxiDma::SetInterruptThreshold(uint8_t thresh, AxiDmaChannel chan) {
  auto dma_chan = (chan == kMm2s) ? &registers_->mm2s : &registers_->s2mm;
  dma_chan->DMACR &= ~(0xff << DMACR_IRQThresh);
  dma_chan->DMACR |= ~(thresh << DMACR_IRQThresh);
}

AxiDma::~AxiDma() {
  close(dev_mem_fd_);
}

void AxiDma::Reset() {
  registers_->s2mm.DMACR |= (1 << DMACR_RESET);
  registers_->mm2s.DMACR |= (1 << DMACR_RESET);
}

void AxiDma::Stop() {
  registers_->mm2s.DMACR &= ~(1 << DMACR_SR);
  registers_->s2mm.DMACR &= ~(1 << DMACR_SR);
}

void AxiDma::ResetDescriptor(std::size_t descriptor_idx) {
  auto& desc = descriptors_.at(descriptor_idx);

  desc.regs->NXTDESC = 0;
  desc.regs->NXTDESC_MSB = 0;
  desc.regs->BUFFER_ADDRESS = 0;
  desc.regs->BUFFER_ADDRESS_MSB = 0;
  desc.regs->CONTROL = 0;
  desc.regs->STATUS = 0;
  desc.regs->APP0 = 0;
  desc.regs->APP1 = 0;
  desc.regs->APP2 = 0;
  desc.regs->APP3 = 0;
  desc.regs->APP4 = 0;
  desc.next = nullptr;
}

void AxiDma::ResetDescriptorStatus(std::size_t descriptor_idx) {
  auto& desc = descriptors_.at(descriptor_idx);

  desc.regs->STATUS = 0;
}

AxiDmaDescriptorStatus AxiDma::GetDescriptorStatus(std::size_t descriptor_idx) const {
  auto& desc = descriptors_.at(descriptor_idx);

  auto stat = desc.regs->STATUS;
  auto ctrl = desc.regs->CONTROL;
  return AxiDmaDescriptorStatus{static_cast<bool>(stat & (1 << DESCSR_Cmplt)),
                                static_cast<bool>(stat & (1 << DESCSR_DMADecErr)),
                                static_cast<bool>(stat & (1 << DESCSR_DMASlvErr)),
                                static_cast<bool>(stat & (1 << DESCSR_DMAIntErr)),
                                static_cast<uint32_t>(ctrl & DESCCR_BufLenMask),
                                static_cast<uint32_t>(stat & DESCSR_TransferredBytesMask)};
}

void AxiDma::PrintDescriptorStatus(std::size_t descriptor_idx) const {
  auto& desc = descriptors_.at(descriptor_idx);
  auto stat = GetDescriptorStatus(descriptor_idx);
  std::cout << "===========================================\n"
            << "raw address: " << std::hex << desc.raw_address << "\n"
            << "buffer address: " << std::hex << desc.regs->BUFFER_ADDRESS << "\n"
            << "completed: " << stat.completed << "\n"
            << "decode error: " << stat.decode_error << "\n"
            << "slave error: " << stat.slave_error << "\n"
            << "internal error: " << stat.internal_error << "\n"
            << "transaction length: " << stat.transaction_len << "\n"
            << "bytes transferred: " << stat.bytes_transferred << "\n"
            << "===========================================\n";
}

void AxiDma::SetDescriptorBufferOffset(std::size_t descriptor_idx, std::size_t offset) {
  auto& desc = descriptors_.at(descriptor_idx);

  // We need to provide both the physical memory address, as well as a pointer to the buffer in
  // virtual memory
  auto buff_phys_addr = kBufferStart + offset;
  auto buff_virt_addr = buffer_ + offset;

  desc.regs->BUFFER_ADDRESS = buff_phys_addr;
  desc.virtual_buffer = buff_virt_addr;
}

void AxiDma::SetDescriptorTxLen(std::size_t descriptor_idx, uint32_t length) {
  auto& desc = descriptors_.at(descriptor_idx);

  // Clear the transaction length field
  desc.regs->CONTROL ^= ~DESCCR_BufLenMask;
  desc.regs->CONTROL |= length & DESCCR_BufLenMask;
}

void AxiDma::SetDescriptorNext(std::size_t descriptor_idx, std::size_t next_descriptor_idx) {
  auto& desc = descriptors_.at(descriptor_idx);
  auto& next = descriptors_.at(next_descriptor_idx);

  desc.next = &next;
  desc.regs->NXTDESC = next.raw_address;
}

std::size_t AxiDma::GetDescriptorNext(std::size_t descriptor_idx) const {
  auto& desc = descriptors_.at(descriptor_idx);

  assert(desc.next);
  auto next_idx = desc.next - descriptors_.data();
  return next_idx;
}

std::string_view AxiDma::GetDescriptorData(std::size_t descriptor_idx) const {
  auto& desc = descriptors_.at(descriptor_idx);
  auto stat = GetDescriptorStatus(descriptor_idx);
  return {desc.virtual_buffer, stat.transaction_len};
}

AxiDmaStatus AxiDma::GetStatus(AxiDmaChannel chan) const {
  auto dma_chan = (chan == kMm2s) ? &registers_->mm2s : &registers_->s2mm;

  auto stat = dma_chan->DMASR;
  return AxiDmaStatus{
      static_cast<bool>(stat & (1 << DMASR_Halted)),
      static_cast<bool>(stat & (1 << DMASR_Idle)),
      static_cast<bool>(stat & (1 << DMASR_SGIncld)),
      static_cast<bool>(stat & (1 << DMASR_IntErr)),
      static_cast<bool>(stat & (1 << DMASR_SlvErr)),
      static_cast<bool>(stat & (1 << DMASR_DecErr)),
      static_cast<bool>(stat & (1 << DMASR_SGIntErr)),
      static_cast<bool>(stat & (1 << DMASR_SGSlvErr)),
      static_cast<bool>(stat & (1 << DMASR_SGDecErr)),
      static_cast<bool>(stat & (1 << DMASR_IOCIrq)),
      static_cast<bool>(stat & (1 << DMASR_DlyIrq)),
      static_cast<bool>(stat & (1 << DMASR_ErrIrq)),
      static_cast<uint8_t>(stat & (0xff << DMASR_IrqThreshSts)),
      static_cast<uint8_t>(stat & (0xff << DMASR_IrqDelaySts)),
  };
}

void AxiDma::PrintStatus(AxiDmaChannel chan) const {
  auto stat = GetStatus(chan);
  std::cout << "halted: " << stat.halted << "\n"
            << "idle: " << stat.idle << "\n"
            << "scatter gather included: " << stat.sg_included << "\n"
            << "dma internal error: " << stat.dma_internal_error << "\n"
            << "dma slave error: " << stat.dma_slave_error << "\n"
            << "dma decode error: " << stat.dma_decode_error << "\n"
            << "sg internal error: " << stat.sg_internal_error << "\n"
            << "sg slave error: " << stat.sg_slave_error << "\n"
            << "sg decode error: " << stat.sg_decode_error << "\n"
            << "complete irq: " << stat.complete_irq << "\n"
            << "delay irq: " << stat.delay_irq << "\n"
            << "error irq: " << stat.error_irq << "\n"
            << "irq threshold status: " << unsigned(stat.irq_threshold_status) << "\n"
            << "irq delay status: " << unsigned(stat.irq_delay_status) << "\n";
}

void AxiDma::ExecuteDescriptorChain(
    std::size_t chain_start_idx,
    AxiDmaChannel chan,
    bool enable_interrupts,
    bool cyclic_operation) {
  // TODO: Chain validation
  // TODO: Address validation (Since we know our DMA memory bounds, we can validate the chain
  // first)
  //
  // TODO: Locking - We shouldn't try to execute multiple chains at
  // once. This should be able to be accomplished by checking the status register

  // From pg021_axi_dma.pdf:
  //
  // 1. Write the address of the starting descriptor to the Current
  // Descriptor register. If AXI DMA is configured for an address
  // space greater than 32, then also program the MSB 32 bits of the
  // current descriptor.
  //
  // 2. Start the channel running by setting the run/stop bit to
  // 1 (DMACR.RS =1).  The halted bit (DMASR.Halted) should
  // deassert indicating the channel is running.
  //
  // 3. If desired, enable interrupts by writing a 1 to
  // DMACR.IOC_IrqEn and DMACR.Err_IrqEn.
  //
  // 4. Write a valid address to the Tail Descriptor register. If AXI
  // DMA is configured for an address space greater than 32, then also
  // program the MSB 32 bits of the current descriptor.
  //
  // 5. Writing to the Tail Descriptor register triggers the DMA to
  // start fetching the descriptors from the memory.

  AxiDmaDescriptor* chain_start = &descriptors_.at(chain_start_idx);

  auto dma_chan = (chan == kMm2s) ? &registers_->mm2s : &registers_->s2mm;

  auto chain_end = chain_start;

  dma_chan->CURDESC = chain_start->raw_address;

  dma_chan->DMACR |= (1 << DMACR_SR); // Run

  if (enable_interrupts) {
    dma_chan->DMACR |=
        ((1 << DMACR_ERRIrqEn) | // Error interrupt
         (1 << DMACR_IOCIrqEn)); // Complete interrupt
  }

  if (cyclic_operation) {
    // NOTE: While we do operate in a cyclic fashion, we
    // intentionally do NOT set the cyclic operation flag in the DMA
    // controller.  This allows us to detect overrun events.

    // 0x50 has no significance, it was simply suggested in the datasheet
    dma_chan->TAILDESC = 0x50;
  } else {
    // If we're not in cyclic mode, find the end of the chain
    while (chain_end->next != nullptr) {
      chain_end = chain_end->next;
    }

    dma_chan->TAILDESC = chain_end->raw_address;
  }
}

// The following is adapted from the newlib-2.0 memcpy routine and is
// necessary to avoid bus errors as our hardware doesn't seem to like
// the system memcpy (potentially due to 64 bit transfers?)
//
// Newlib is licensed under various MIT/BSD like licenses, which can
// be viewed at the following URL:
// https://www.sourceware.org/git?p=newlib-htdocs.git;a=blob_plain;f=COPYING.NEWLIB;hb=a8e2e3756023848a3b3b4405197d0090d4591300

// Nonzero if either X or Y is not aligned on a "uint32_t" boundary.
#define UNALIGNED(X, Y) \
  (((intptr_t)X & (sizeof(uint32_t) - 1)) | ((intptr_t)Y & (sizeof(uint32_t) - 1)))

// How many bytes are copied each iteration of the 4X unrolled loop.
#define BIGBLOCKSIZE (sizeof(uint32_t) << 2)

// How many bytes are copied each iteration of the word copy loop.
#define LITTLEBLOCKSIZE (sizeof(uint32_t))

// Threshhold for punting to the byte copier.
#define TOO_SMALL(LEN) ((LEN) < BIGBLOCKSIZE)

char* AxiDma::MemCpy(char* dst0, const char* src0, size_t len0) {
  char* dst = dst0;
  const char* src = src0;
  uint32_t* aligned_dst;
  const uint32_t* aligned_src;

  // If the size is small, or either SRC or DST is unaligned, then
  // punt into the byte copy loop.  This should be rare.
  if (!TOO_SMALL(len0) && !UNALIGNED(src, dst)) {
    aligned_dst = (uint32_t*)dst;
    aligned_src = (uint32_t*)src;

    // Copy 4X long 32 bit words at a time if possible.
    while (len0 >= BIGBLOCKSIZE) {
      *aligned_dst++ = *aligned_src++;
      *aligned_dst++ = *aligned_src++;
      *aligned_dst++ = *aligned_src++;
      *aligned_dst++ = *aligned_src++;
      len0 -= BIGBLOCKSIZE;
    }

    // Copy one 32 bit word at a time if possible.
    while (len0 >= LITTLEBLOCKSIZE) {
      *aligned_dst++ = *aligned_src++;
      len0 -= LITTLEBLOCKSIZE;
    }

    // Pick up any residual with a byte copier.
    dst = (char*)aligned_dst;
    src = (char*)aligned_src;
  }

  while (len0--) {
    *dst++ = *src++;
  }

  return dst;
}

} // namespace libpandobox
} // namespace pnd
