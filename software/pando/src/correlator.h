#pragma once

#include <cstdint>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

#include "histogram.h"
#include "types.h"

namespace pnd {
namespace pando {

/** Class implementing time domain g2 calculation using the exponential rebinning strategy.
 * @tparam BinDuration specialization of std::duration to be used for the demarcation of bin edges
 */
template <class BinDuration>
class Correlator {
 public:
  class CorrelatorException : public std::runtime_error {
   public:
    using std::runtime_error::runtime_error;
  };

  /** Type returned by UpdateG2. */
  struct Result {
    explicit Result(size_t size) : tau_k(size), g2(size){};
    /** g2 X values (lag times). */
    DurationVector<typename BinDuration::rep, typename BinDuration::period> tau_k;

    /** g2 Y values. */
    std::vector<double> g2;
  };

  /** Public constructor.
   * @param bin_size The starting bin width
   * @param points_per_level The number of points to compute before rebinning and proceding to the
   * next level
   * @param n_levels (number of times to rebin + 1)
   * @param rebin_factor Number of adjacent bins to condense into one at each rebinning step
   */
  Correlator(BinDuration bin_size, size_t points_per_level, size_t n_levels, size_t rebin_factor)
      : bin_size_{bin_size},
        points_per_level_{points_per_level},
        n_levels_{n_levels},
        rebin_factor_{rebin_factor},
        first_offset_{0},
        g2_{std::make_unique<Result>(
            points_per_level_ +
            (n_levels - 1) *
                (points_per_level_ - (points_per_level_ + rebin_factor_ - 1) / rebin_factor_))},
        tauk_begin_it_{g2_->tau_k.begin()},
        g2_begin_it_{g2_->g2.begin()} {
    MakeChildren();
  }

  /** Compute g2 using the parameters provided in the constructor.
   * @param binned Binned macro times from a single channel
   * @return g2 calculation result
   */
  const Result& UpdateG2(const Histogram<BinDuration>& binned) {
    if (binned.GetBinSize() != bin_size_) {
      throw std::invalid_argument("Input has wrong bin size");
    }

    UpdateG2Impl(binned);
    return *g2_;
  }

  /** Get the most recently computed g2. */
  const Result& GetResult() const {
    return *g2_;
  }

 private:
  /** Private constructor used by MakeChildren. */
  Correlator(
      BinDuration bin_size,
      size_t points_per_level,
      size_t n_levels,
      size_t rebin_factor,
      size_t first_offset,
      typename decltype(((Result*)nullptr)->tau_k)::iterator tauk_begin_it,
      typename decltype(((Result*)nullptr)->g2)::iterator g2_begin_it)
      : bin_size_{bin_size},
        points_per_level_{points_per_level},
        n_levels_{n_levels},
        rebin_factor_{rebin_factor},
        first_offset_{first_offset},
        tauk_begin_it_{tauk_begin_it},
        g2_begin_it_{g2_begin_it} {
    MakeChildren();
  }

  void MakeChildren() {
    if (n_levels_ > 1) {
      auto child = new Correlator(
          bin_size_ * rebin_factor_,
          points_per_level_,
          n_levels_ - 1,
          rebin_factor_,
          (points_per_level_ + rebin_factor_ - 1) / rebin_factor_,
          tauk_begin_it_ + points_per_level_ - first_offset_,
          g2_begin_it_ + points_per_level_ - first_offset_);

      child_ = std::unique_ptr<Correlator>(child);
    }
  }

  void UpdateG2Impl(const Histogram<BinDuration>& binned) {
    // Compute g2 for higher levels, if any
    if (child_) {
      rebinned_.Rebin(binned);
      child_->UpdateG2Impl(rebinned_);
    }

    size_t bin_count = binned.size();

    if (bin_count < points_per_level_) {
      throw CorrelatorException("Too few bins to compute correlation at maximum offset");
    }

    auto g2_it = g2_begin_it_;
    auto tau_k_it = tauk_begin_it_;

    // Produce g2 from binned
    for (size_t k = first_offset_; k < points_per_level_; ++k) {
      auto f_i = binned.cbegin();
      auto f_ik = binned.cbegin() + k;

      int64_t prod_accum = 0;
      int64_t left_accum = 0;
      int64_t right_accum = 0;

      // TODO benchmark, comapare to eigen, etc.
      for (size_t i = 0; i < bin_count - k; ++i) {
        prod_accum += (*f_i) * (*f_ik);
        left_accum += *f_i;
        right_accum += *f_ik;

        ++f_i;
        ++f_ik;
      }

      // Normalize
      // Preempt divide-by-zero by forcing result to 1.0
      double g2_x;
      if (left_accum && right_accum) {
        g2_x = (bin_count - k) * prod_accum;
        g2_x /= (left_accum * right_accum);
      } else {
        g2_x = 1.0;
      }

      *(g2_it++) = g2_x;
      *(tau_k_it++) = k * bin_size_;
    }
  }

  const BinDuration bin_size_;
  const size_t points_per_level_;
  const size_t n_levels_;
  const size_t rebin_factor_;

  const size_t first_offset_;

  Histogram<BinDuration> rebinned_{bin_size_ * rebin_factor_};

  std::unique_ptr<Result> g2_;
  const typename decltype(((Result*)nullptr)->tau_k)::iterator tauk_begin_it_;
  const typename decltype(((Result*)nullptr)->g2)::iterator g2_begin_it_;

  std::unique_ptr<Correlator> child_;
};

} // namespace pando
} // namespace pnd
