#pragma once

#include <pybind11/numpy.h>

#include <cstddef> // std::size_t
#include <cstdint> // int16_t, int32_t
#include <deque>
#include <vector>

namespace py = pybind11;

class ContrastCalculator {
 public:
  using PixelValue = float;
  using ContrastValue = float;

  /** Constructor.
   * @param n_pixels The size of each frame, in pixels.
   */
  ContrastCalculator();

  py::array_t<double> GetAllContrasts(
      py::array_t<PixelValue> frame0,
      py::array_t<PixelValue> frame1,
      py::array_t<PixelValue> frame2,
      py::array_t<PixelValue> frame3,
      bool frame0_active,
      bool frame1_active,
      bool frame2_active,
      bool frame3_active);

  double GetContrast(int n_pixels, PixelValue* frame_ptr);
  static float Sum(int start_pixel, int stop_pixel, PixelValue* frame_ptr);
  static float
  SquareDistFromMean(int start_pixel, int stop_pixel, float mean, PixelValue* frame_ptr);
};