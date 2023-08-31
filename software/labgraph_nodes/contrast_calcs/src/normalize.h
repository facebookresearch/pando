#include <pybind11/numpy.h>

namespace py = pybind11;

/** Normalize new frame by rolling average, and place result in norm. */
void normalize(
    py::array_t<std::uint8_t> avg,
    py::array_t<std::uint8_t> new_fr,
    py::array_t<float> norm);

/** auxillary function for parallelizing normalize function */
void norm_for_loop_func(
    int start_pixel,
    int stop_pixel,
    std::uint8_t* avg_ptr,
    std::uint8_t* new_fr_ptr,
    float* norm_ptr);
