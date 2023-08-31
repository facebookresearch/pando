#pragma once

#include <chrono>
#include <cstdint>
#include <ratio>
#include <utility>
#include <vector>

namespace pnd {
namespace pando {

/** Custom helper type, since there is no std::chrono::picoseconds. */
using picoseconds = std::chrono::duration<int64_t, std::pico>;

/** A std::vector of std::durations, with custom iterators for the underlying Rep type
 * @tparam Rep Storage type for each timestamp representing a number of ticks
 * @tparam Period a std::ratio representing the tick period for each timestamp
 */
template <class Rep, class Period>
struct DurationVector : public std::vector<std::chrono::duration<Rep, Period>> {
  using std::vector<std::chrono::duration<Rep, Period>>::vector;

  /** @return a const pointer to the first stored timestamp casted Rep*  */
  const Rep* RepCBegin() const {
    return reinterpret_cast<const Rep*>(this->data());
  }

  /** @return a const pointer to the (last+1) stored timestamp casted to Rep*  */
  const Rep* RepCEnd() const {
    return reinterpret_cast<const Rep*>(this->data() + this->size());
  }
};

/** Specialization of DurationVector for storing macro times. */
using MacroTimes = DurationVector<uint64_t, std::pico>;

/** Specialization of DurationVector for storing micro times. */
using MicroTimes = DurationVector<uint32_t, std::pico>;

/** Aliases for single [macro/micro]time */
using MicroTime = typename MicroTimes::value_type;
using MacroTime = typename MacroTimes::value_type;

/** Storage for macro and micro times from a single channel.
 * Macro and micro times are stored as uint64_t and uint32_t, respectively
 */
struct ChannelTimestamps : std::pair<MacroTimes, MicroTimes> {
  /* @return const reference to the stored macro times */
  const MacroTimes& macro_times() const {
    return first;
  }

  /* @return Reference to the stored macro times */
  MacroTimes& macro_times() {
    return first;
  }

  /* @return const reference to the stored micro times */
  const MicroTimes& micro_times() const {
    return second;
  }

  /* @return Reference to the stored micro times */
  MicroTimes& micro_times() {
    return second;
  }

  /** Specify the span of time represented by the stored timestamps.
   * All stored macrotimes shall fall within the range [from, until).
   * @param from Span start time (inclusive)
   * @param until Span end time (exclusive)
   */
  void SetSpan(MacroTime from, MacroTime until) {
    span_from_ = from;
    span_until_ = until;
  }

  auto GetSpanFrom() const {
    return span_from_;
  }

  auto GetSpanUntil() const {
    return span_until_;
  }

  MacroTime span_from_;
  MacroTime span_until_;
};

} // namespace pando
} // namespace pnd
