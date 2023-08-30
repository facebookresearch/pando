#include "normalize.h"

#include <future>

void normalize(
    py::array_t<std::uint8_t> avg,
    py::array_t<std::uint8_t> new_fr,
    py::array_t<float> norm) {
  py::buffer_info avg_buf = avg.request();
  py::buffer_info new_fr_buf = new_fr.request();
  py::buffer_info norm_buf = norm.request();

  // get raw frame data
  // assuming avg and new_fr consist of same type
  auto* avg_ptr = static_cast<std::uint8_t*>(avg_buf.ptr);
  auto* new_fr_ptr = static_cast<std::uint8_t*>(new_fr_buf.ptr);
  auto* norm_ptr = static_cast<float*>(norm_buf.ptr);

  int n_pixels = static_cast<int>(avg.size());

  int increment = 262144;
  std::vector<std::future<void>> l_norm;

  for (int i = 0; i < n_pixels; i += increment) {
    int stop_pixel = std::min(i + increment, n_pixels);
    l_norm.push_back(std::async(norm_for_loop_func, i, stop_pixel, avg_ptr, new_fr_ptr, norm_ptr));
  }
  for (int i = 0; i < l_norm.size(); i++) {
    l_norm[i].get();
  }

  return;
}

void norm_for_loop_func(
    int start_pixel,
    int stop_pixel,
    std::uint8_t* avg_ptr,
    std::uint8_t* new_fr_ptr,
    float* norm_ptr) {
  for (size_t idx = start_pixel; idx < stop_pixel; idx++) {
    if (avg_ptr[idx] == 0) {
      norm_ptr[idx] = 0;
    } else {
      norm_ptr[idx] = (float)new_fr_ptr[idx] / avg_ptr[idx];
    }
  }
  return;
}