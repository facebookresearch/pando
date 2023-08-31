#include "contrast_calculator.h"
#include "dark_image.h"
#include "normalize.h"
#include "rolling_average.h"

#include <pybind11/pybind11.h>

namespace py = pybind11;

// This is the definition of a python module named test_module
PYBIND11_MODULE(contrast_calcs, m) {
  // The contains a member function add_arrays, which is implmented by a c++
  // function of the same name.
  m.def("normalize", &normalize, "Normalize new frame by rolling average");
  m.def("dark_image", &dark_image, "Subtract dark image from frame");

  py::class_<RollingAverage>(m, "RollingAverage")
      .def(py::init<std::uint32_t>())
      .def("Update", &RollingAverage::Update);

  py::class_<ContrastCalculator>(m, "ContrastCalculator")
      .def(py::init())
      .def("GetAllContrasts", &ContrastCalculator::GetAllContrasts, py::return_value_policy::move)
      .def("GetContrast", &ContrastCalculator::GetContrast);
}
