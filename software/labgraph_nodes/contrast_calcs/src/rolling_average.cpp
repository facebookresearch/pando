
#include "rolling_average.h"

#include <cassert>
#include <future>
#include <iostream>
#include <stdexcept>
#include <utility>
#include "pybind11/pytypes.h"

RollingAverage::RollingAverage(std::uint32_t n_pixels) : n_pixels_{n_pixels} {
  // Allocate storage
  accum_.assign(n_pixels_, 0);
  window_avg_.assign(n_pixels_, 0);
}

py::array_t<RollingAverage::PixelValue> RollingAverage::Update(
    py::array_t<PixelValue> frame,
    std::uint16_t window_size) {
  // Validate arguments
  if (frame.size() != n_pixels_)
    throw std::runtime_error("frame has wrong size");

  // Get raw frame data
  py::buffer_info frame_buff = frame.request();
  auto* frame_ptr = static_cast<PixelValue*>(frame_buff.ptr);

  // Handle window shrink
  while (frame_history_.size() > window_size + 1) {
    auto& oldest_frame = frame_history_.back();
    const auto n_pixels = n_pixels_;
    for (std::size_t i = 0; i < n_pixels; ++i)
      accum_[i] -= oldest_frame[i];
    frame_history_.pop_back();
  }

  if (frame_history_.size() > window_size + 1)
    frame_history_.erase(frame_history_.begin() + window_size, frame_history_.end());

  // The number of frames currently in the accumulator
  std::uint16_t n_frames = static_cast<std::uint16_t>(frame_history_.size());

  std::vector<PixelValue> new_history;

  int increment = 131072;
  if (n_frames == (window_size + 1)) {
    if (initial_expansion)
      initial_expansion = false;
    auto oldest_frame = std::move(frame_history_.back());
    frame_history_.pop_back();

    // reuse storage from oldest_frame (can't do this in expand case)
    new_history = std::move(oldest_frame);

    // Copy frame into frame_history_
    new_history.assign(frame_ptr, frame_ptr + n_pixels_);
    frame_history_.push_front(std::move(new_history));

    const int n_pixels = n_pixels_;
    std::vector<std::future<void>> l_move;

    for (int i = 0; i < n_pixels; i += increment) {
      int stop_pixel = std::min(i + increment, n_pixels);
      l_move.push_back(std::async(&RollingAverage::MoveWindow, this, i, stop_pixel));
    }
    for (int i = 0; i < l_move.size(); i++) {
      l_move[i].get();
    }
  } else if (n_frames == window_size && initial_expansion) {
    const int n_pixels = n_pixels_;

    // Copy frame into frame_history_
    new_history.assign(frame_ptr, frame_ptr + n_pixels_);
    frame_history_.push_front(std::move(new_history));

    std::vector<std::future<void>> l_move;

    for (int i = 0; i < n_pixels; i += increment) {
      int stop_pixel = std::min(i + increment, n_pixels);
      l_move.push_back(std::async(&RollingAverage::MoveWindow, this, i, stop_pixel));
    }
    for (int i = 0; i < l_move.size(); i++) {
      l_move[i].get();
    }
  } else {
    const int n_pixels = n_pixels_;

    // Copy frame into frame_history_
    new_history.assign(frame_ptr, frame_ptr + n_pixels_);
    frame_history_.push_front(std::move(new_history));

    std::vector<std::future<void>> l_exp;

    for (int i = 0; i < n_pixels; i += increment) {
      int stop_pixel = std::min(i + increment, n_pixels);
      l_exp.push_back(std::async(&RollingAverage::ExpandWindow, this, i, stop_pixel));
    }
    for (int i = 0; i < l_exp.size(); i++) {
      l_exp[i].get();
    }
  }

  return py::array_t<PixelValue>(
      frame_buff.shape, // copy shape from input frame
      window_avg_.data());
}

void RollingAverage::MoveWindow(int start_pixel, int stop_pixel) {
  std::uint16_t n_frames = static_cast<std::uint16_t>(frame_history_.size());

  for (std::size_t i = start_pixel; i < stop_pixel; ++i) {
    accum_[i] -= frame_history_.back()[i];
    accum_[i] += frame_history_.front()[i];
    // expect frame history to be one longer than window len
    window_avg_[i] = accum_[i] / (n_frames - 1);
  }
}

void RollingAverage::ExpandWindow(int start_pixel, int stop_pixel) {
  std::uint16_t n_frames = static_cast<std::uint16_t>(frame_history_.size());

  for (std::size_t i = start_pixel; i < stop_pixel; ++i) {
    accum_[i] += frame_history_.front()[i];
    if (initial_expansion)
      window_avg_[i] = accum_[i] / n_frames;
    else
      window_avg_[i] = accum_[i] / (n_frames - 1);
  }
}