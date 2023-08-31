#include <pybind11/numpy.h>

namespace py = pybind11;

/** Subtract the dark image from the frame */
void dark_image(py::array_t<std::uint8_t> dark_image, py::array_t<std::uint8_t> frame);

/** auxillary function for parallelizing subtraction */
void subtract_for_loop_func(
    int start_pixel,
    int stop_pixel,
    std::uint8_t* dark_image_ptr,
    std::uint8_t* frame_ptr);