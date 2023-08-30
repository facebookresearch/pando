#include "contrast_calculator.h"

#include <cassert>
#include <cmath>
#include <future>
#include <iostream>
#include <stdexcept>
#include <utility>
#include "pybind11/pytypes.h"

ContrastCalculator::ContrastCalculator() {}

py::array_t<double> ContrastCalculator::GetAllContrasts(
    py::array_t<ContrastCalculator::PixelValue> frame0,
    py::array_t<ContrastCalculator::PixelValue> frame1,
    py::array_t<ContrastCalculator::PixelValue> frame2,
    py::array_t<ContrastCalculator::PixelValue> frame3,
    bool frame0_active,
    bool frame1_active,
    bool frame2_active,
    bool frame3_active) {
  std::vector<std::future<double>> l_contrasts;
  double contrast_vals[4] = {0.0, 0.0, 0.0, 0.0};

  // Get raw frame data
  py::buffer_info frame0_buff = frame0.request();
  auto* frame0_ptr = static_cast<PixelValue*>(frame0_buff.ptr);
  int n_pixels_fr0 = static_cast<int>(frame0.size());

  py::buffer_info frame1_buff = frame1.request();
  auto* frame1_ptr = static_cast<PixelValue*>(frame1_buff.ptr);
  int n_pixels_fr1 = static_cast<int>(frame1.size());

  py::buffer_info frame2_buff = frame2.request();
  auto* frame2_ptr = static_cast<PixelValue*>(frame2_buff.ptr);
  int n_pixels_fr2 = static_cast<int>(frame2.size());

  py::buffer_info frame3_buff = frame3.request();
  auto* frame3_ptr = static_cast<PixelValue*>(frame3_buff.ptr);
  int n_pixels_fr3 = static_cast<int>(frame3.size());

  if (frame0_active) {
    l_contrasts.push_back(
        std::async(&ContrastCalculator::GetContrast, this, n_pixels_fr0, frame0_ptr));
  }
  if (frame1_active) {
    l_contrasts.push_back(
        std::async(&ContrastCalculator::GetContrast, this, n_pixels_fr1, frame1_ptr));
  }
  if (frame2_active) {
    l_contrasts.push_back(
        std::async(&ContrastCalculator::GetContrast, this, n_pixels_fr2, frame2_ptr));
  }
  if (frame3_active) {
    l_contrasts.push_back(
        std::async(&ContrastCalculator::GetContrast, this, n_pixels_fr3, frame3_ptr));
  }

  if (frame0_active) {
    contrast_vals[0] = l_contrasts[0].get();
  }
  if (frame1_active) {
    contrast_vals[1] = l_contrasts[1].get();
  }
  if (frame2_active) {
    contrast_vals[2] = l_contrasts[2].get();
  }
  if (frame3_active) {
    contrast_vals[3] = l_contrasts[3].get();
  }

  // return contrast_vals;
  // std::cout << contrast_vals << "\n";
  return py::array_t<double>({4}, contrast_vals);
}

double ContrastCalculator::GetContrast(int n_pixels, PixelValue* frame_ptr) {
  // initialize values
  float sum = 0;
  float mean = 0;
  float sqdist = 0;
  int increment = 524288;
  std::vector<std::future<float>> l_sum;

  for (int i = 0; i < n_pixels; i += increment) {
    int stop_pixel = std::min(i + increment, n_pixels);
    l_sum.push_back(std::async(&ContrastCalculator::Sum, i, stop_pixel, frame_ptr));
  }
  for (int i = 0; i < l_sum.size(); i++) {
    sum += l_sum[i].get();
  }

  std::vector<std::future<float>> l_sqdist;

  mean = (sum / n_pixels);

  for (int i = 0; i < n_pixels; i += increment) {
    int stop_pixel = std::min(i + increment, n_pixels);
    l_sqdist.push_back(
        std::async(&ContrastCalculator::SquareDistFromMean, i, stop_pixel, mean, frame_ptr));
  }
  for (int i = 0; i < l_sqdist.size(); i++) {
    sqdist += l_sqdist[i].get();
  }

  return sqrt(sqdist / n_pixels) / mean;
}

float ContrastCalculator::Sum(int start_pixel, int stop_pixel, PixelValue* frame_ptr) {
  float sum = 0;
  for (std::size_t i = start_pixel; i < stop_pixel; ++i) {
    sum += frame_ptr[i];
  }
  return sum;
}

float ContrastCalculator::SquareDistFromMean(
    int start_pixel,
    int stop_pixel,
    float mean,
    PixelValue* frame_ptr) {
  float sqdist = 0;
  for (std::size_t i = start_pixel; i < stop_pixel; ++i) {
    sqdist += static_cast<float>((frame_ptr[i] - mean) * (frame_ptr[i] - mean));
  }
  return sqdist;
}