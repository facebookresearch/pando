// Copyright 2004-present Facebook. All Rights Reserved.
// BCI C++ system configuration

#include "system_configuration.h"

#include <boost/math/constants/constants.hpp>
#include <logging/Checks.h>
#include <cmath>
#include <tuple>

namespace bci {
namespace systems {
namespace configuration {

SourceGroup::SourceGroup(const YAML::Node& sourcesNode, size_t sourcesOffset, double wavelength)
    : sourcesOffset_(sourcesOffset), numSources_(0), wavelength_(wavelength) {
  for (YAML::const_iterator sourceIterator = sourcesNode.begin();
       sourceIterator != sourcesNode.end();
       ++sourceIterator) {
    sources_.push_back(sourceIterator->as<YAML::Node>()["coordinate"].as<size_t>());
  }
  numSources_ = sources_.size();
}

Canister::Canister(
    const YAML::Node& canisterNode,
    size_t coordinatesOffset,
    size_t sourcesOffset,
    size_t detectorsOffset,
    const std::string& configVersion)
    : canisterName_(canisterNode["Name"].as<std::string>()),
      coordinatesOffset_(coordinatesOffset),
      sourcesOffset_(sourcesOffset),
      detectorsOffset_(detectorsOffset),
      numCoordinates_(0),
      numSources_(0),
      numDetectors_(0) {
  const YAML::Node& sourcesNode = canisterNode["Sources"];

  if (configVersion != "1.0.0") {
    XR_CHECK(sourcesNode["Wavelengths"], "Missing required field: Wavelengths");
  }
  if (sourcesNode["Wavelengths"]) {
    const YAML::Node& wavelengthsNode = sourcesNode["Wavelengths"];
    for (YAML::const_iterator wavelengthsIt = wavelengthsNode.begin();
         wavelengthsIt != wavelengthsNode.end();
         ++wavelengthsIt) {
      wavelengths_.emplace(
          wavelengthsIt->first.as<std::string>(), wavelengthsIt->second.as<double>());
    }
  }

  for (YAML::const_iterator sourceGroupIterator = sourcesNode.begin();
       sourceGroupIterator != sourcesNode.end();
       ++sourceGroupIterator) {
    // Skip wavelengths.
    if (sourceGroupIterator->first.as<std::string>() == "Wavelengths") {
      continue;
    }

    double wavelength = -1.0;
    // TODO(rsevile): Replace config string with 3 integers.
    if (configVersion != "1.0.0") {
      XR_CHECK(
          wavelengths_.count(sourceGroupIterator->first.as<std::string>()) == 1,
          "Missing required field {} in dict Wavelengths",
          sourceGroupIterator->first.as<std::string>());
    }
    if (wavelengths_.count(sourceGroupIterator->first.as<std::string>()) == 1) {
      wavelength = wavelengths_.at(sourceGroupIterator->first.as<std::string>());
    }
    // The numSources_ counter contains the current offset,
    // so it can be reused as the offset here.
    sourceGroups_.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(sourceGroupIterator->first.as<std::string>()),
        std::forward_as_tuple(
            sourcesNode[sourceGroupIterator->first.as<std::string>()],
            sourcesOffset_ + numSources_,
            wavelength));

    numSources_ +=
        sourceGroups_.find(sourceGroupIterator->first.as<std::string>())->second.getNumSources();
  }
  coordinates_ = canisterNode["Coordinates"].as<std::vector<std::array<double, 3>>>();
  if (canisterNode["Transformation"]) {
    if (canisterNode["Transformation"]["AffineMatrix"]) {
      for (std::array<double, 3>& coordinate : coordinates_) {
        // Homogeneous coordinates affine transformation
        // http://graphics.cs.cmu.edu/nsp/course/15-462/Spring04/slides/04-transform.pdf
        matrix_multiply(
            coordinate,
            canisterNode["Transformation"]["AffineMatrix"]
                .as<std::array<std::array<double, 4>, 4>>());
      }
    } else if (canisterNode["Transformation"]["EulerAngles"]) {
      // Apply Euler angles transformation
      // https://en.wikipedia.org/wiki/Euler_angles
      // Rotations are applied in ZYX order first,
      // then translation is applied.

      // Apply rotation matrices
      // https://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations
      std::array<double, 3> rotation =
          canisterNode["Transformation"]["EulerAngles"]["Rotation"].as<std::array<double, 3>>();

      if (canisterNode["Transformation"]["EulerAngles"]["Units"].as<std::string>() == "degrees") {
        for (int i = 0; i < 3; ++i) {
          rotation[i] = rotation[i] * boost::math::constants::pi<double>() / 180.0;
        }
      }

      std::array<double, 3> cosines = {
          std::cos(rotation[0]), std::cos(rotation[1]), std::cos(rotation[2])};

      std::array<double, 3> sines = {
          std::sin(rotation[0]), std::sin(rotation[1]), std::sin(rotation[2])};

      std::array<std::array<double, 3>, 3> zMatrix = {{
          {cosines[2], -sines[2], 0.0},
          {sines[2], cosines[2], 0.0},
          {0.0, 0.0, 1.0},
      }};

      std::array<std::array<double, 3>, 3> yMatrix = {{
          {cosines[1], 0.0, -sines[1]},
          {0.0, 1.0, 0.0},
          {-sines[1], 0.0, cosines[1]},
      }};

      std::array<std::array<double, 3>, 3> xMatrix = {{
          {1.0, 0.0, 0.0},
          {0.0, cosines[0], -sines[0]},
          {0.0, sines[0], cosines[0]},
      }};

      std::array<std::array<double, 3>, 3> yzMatrix;

      matrix_multiply(yzMatrix, yMatrix, zMatrix);

      std::array<std::array<double, 3>, 3> staticMatrix;

      matrix_multiply(staticMatrix, xMatrix, yzMatrix);

      std::array<double, 3> translation =
          canisterNode["Transformation"]["EulerAngles"]["Translation"].as<std::array<double, 3>>();

      for (std::array<double, 3>& coordinate : coordinates_) {
        matrix_multiply(coordinate, staticMatrix);
        for (int i = 0; i < 3; ++i) {
          coordinate[i] += translation[i];
        }
      }
    }
  }
  for (YAML::const_iterator detectorIterator = canisterNode["Detectors"].begin();
       detectorIterator != canisterNode["Detectors"].end();
       ++detectorIterator) {
    detectors_.push_back(detectorIterator->as<YAML::Node>()["coordinate"].as<size_t>());
  }
  numCoordinates_ = coordinates_.size();
  numDetectors_ = detectors_.size();
}

void Canister::matrix_multiply(
    std::array<std::array<double, 3>, 3>& result,
    const std::array<std::array<double, 3>, 3>& leftMatrix,
    const std::array<std::array<double, 3>, 3>& rightMatrix) {
  for (int row = 0; row < 3; ++row) {
    // Dot product with the row
    for (int column = 0; column < 3; ++column) {
      result[row][column] = 0.0;
      for (int i = 0; i < 3; ++i) {
        result[row][column] += leftMatrix[row][i] * rightMatrix[i][column];
      }
    }
  }
}

void Canister::matrix_multiply(
    std::array<double, 3>& coordinate,
    const std::array<std::array<double, 3>, 3>& matrix) {
  std::array<double, 3> result_coordinate = {0.0, 0.0, 0.0};

  for (int row = 0; row < 3; ++row) {
    // Dot product with the row
    for (int column = 0; column < 3; ++column) {
      result_coordinate[row] += matrix[row][column] * coordinate[column];
    }
  }

  for (int i = 0; i < 3; ++i) {
    coordinate[i] = result_coordinate[i];
  }
}

void Canister::matrix_multiply(
    std::array<double, 3>& coordinate,
    const std::array<std::array<double, 4>, 4>& matrix) {
  std::array<double, 4> homogeneous_coordinate = {0.0, 0.0, 0.0, 0.0};

  for (int row = 0; row < 4; ++row) {
    // Dot product with the row
    for (int column = 0; column < 4; ++column) {
      if (column < 3) {
        homogeneous_coordinate[row] += matrix[row][column] * coordinate[column];
      } else {
        homogeneous_coordinate[row] += matrix[row][column];
      }
    }
  }

  for (int i = 0; i < 3; ++i) {
    coordinate[i] = homogeneous_coordinate[i] / homogeneous_coordinate[3];
  }
}

SystemConfiguration::SystemConfiguration(const YAML::Node& rootNode)
    : configVersion_(rootNode["ConfigVersion"].as<std::string>()),
      deviceName_(rootNode["DeviceName"].as<std::string>()),
      numCoordinates_(0),
      numSources_(0),
      numDetectors_(0) {
  for (const auto& canisterNode : rootNode["Canisters"]) {
    // The counters contain the current offsets, so they
    // can be reused as offset counters too.
    canisters_.emplace_back(
        canisterNode, numCoordinates_, numSources_, numDetectors_, configVersion_);

    numCoordinates_ += canisters_.back().getNumCoordinates();
    numSources_ += canisters_.back().getNumSources();
    numDetectors_ += canisters_.back().getNumDetectors();
  }
}

} // namespace configuration
} // namespace systems
} // namespace bci
