#include "dark_image.h"

#include <future>

void dark_image(py::array_t<std::uint8_t> dark_image, py::array_t<std::uint8_t> frame) {
  py::buffer_info dark_image_buf = dark_image.request();
  py::buffer_info frame_buf = frame.request();

  // get raw frame data
  // assuming avg and new_fr consist of same type
  auto* dark_image_ptr = static_cast<std::uint8_t*>(dark_image_buf.ptr);
  auto* frame_ptr = static_cast<std::uint8_t*>(frame_buf.ptr);

  int n_pixels = static_cast<int>(dark_image.size());

  int increment = 32768;
  std::vector<std::future<void>> l_sub;

  for (int i = 0; i < n_pixels; i += increment) {
    int stop_pixel = std::min(i + increment, n_pixels);
    l_sub.push_back(std::async(subtract_for_loop_func, i, stop_pixel, dark_image_ptr, frame_ptr));
  }
  for (int i = 0; i < l_sub.size(); i++) {
    l_sub[i].get();
  }

  return;
}

void subtract_for_loop_func(
    int start_pixel,
    int stop_pixel,
    std::uint8_t* dark_image_ptr,
    std::uint8_t* frame_ptr) {
  for (size_t idx = start_pixel; idx < stop_pixel; idx++) {
    frame_ptr[idx] = std::max(frame_ptr[idx] - dark_image_ptr[idx], 0);
  }
  return;
}