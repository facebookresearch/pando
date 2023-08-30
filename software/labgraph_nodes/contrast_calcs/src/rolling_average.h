#pragma once

#include <pybind11/numpy.h>

#include <cstddef> // std::size_t
#include <cstdint> // int16_t, int32_t
#include <deque>
#include <vector>

namespace py = pybind11;

class RollingAverage {
 public:
  using PixelValue = uint8_t;
  using AccumValue = uint32_t;

  /** Constructor.
   * @param n_pixels The size of each frame, in pixels.
   */
  RollingAverage(std::uint32_t n_pixels);

  py::array_t<PixelValue> Update(py::array_t<PixelValue> frame, std::uint16_t window_size);
  void MoveWindow(int start_pixel, int stop_pixel);
  void ExpandWindow(int start_pixel, int stop_pixel);

 private:
  std::uint32_t n_pixels_;
  std::deque<std::vector<PixelValue>> frame_history_;
  std::vector<AccumValue> accum_;
  std::vector<PixelValue> window_avg_;
  bool initial_expansion = true;
};