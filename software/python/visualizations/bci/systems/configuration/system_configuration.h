// Copyright 2004-present Facebook. All Rights Reserved.
// BCI C++ system configuration

#pragma once

#include <string>
#include <unordered_map>

#include <yaml-cpp/yaml.h>

namespace bci {
namespace systems {
namespace configuration {

class SourceGroup {
 public:
  SourceGroup(const YAML::Node& sourcesNode, size_t sourcesOffset, double wavelength);

  const std::vector<size_t>& getSources() const {
    return sources_;
  }

  size_t getSourcesOffset() const {
    return sourcesOffset_;
  }

  size_t getNumSources() const {
    return numSources_;
  }

  double getWavelength() const {
    return wavelength_;
  }

 private:
  std::vector<size_t> sources_;
  size_t sourcesOffset_;
  size_t numSources_;
  double wavelength_;
};

class Canister {
 public:
  Canister(
      const YAML::Node& canisterNode,
      size_t coordinatesOffset,
      size_t sourcesOffset,
      size_t detectorsOffset,
      const std::string& configVersion);

  const std::string& getCanisterName() const {
    return canisterName_;
  }

  const std::unordered_map<std::string, double>& getWavelengths() const {
    return wavelengths_;
  }

  const std::vector<std::array<double, 3>>& getCoordinates() const {
    return coordinates_;
  }

  const std::map<std::string, SourceGroup>& getSourceGroups() const {
    return sourceGroups_;
  }

  const std::vector<size_t>& getDetectors() const {
    return detectors_;
  }

  size_t getCoordinatesOffset() const {
    return coordinatesOffset_;
  }

  size_t getSourcesOffset() const {
    return sourcesOffset_;
  }

  size_t getDetectorsOffset() const {
    return detectorsOffset_;
  }

  size_t getNumCoordinates() const {
    return numCoordinates_;
  }

  size_t getNumSources() const {
    return numSources_;
  }

  size_t getNumDetectors() const {
    return numDetectors_;
  }

 private:
  void matrix_multiply(
      std::array<std::array<double, 3>, 3>& result,
      const std::array<std::array<double, 3>, 3>& leftMatrix,
      const std::array<std::array<double, 3>, 3>& rightMatrix);
  void matrix_multiply(
      std::array<double, 3>& coordinate,
      const std::array<std::array<double, 3>, 3>& matrix);
  void matrix_multiply(
      std::array<double, 3>& coordinate,
      const std::array<std::array<double, 4>, 4>& matrix);

  std::string canisterName_;
  std::unordered_map<std::string, double> wavelengths_;
  std::vector<std::array<double, 3>> coordinates_;
  std::map<std::string, SourceGroup> sourceGroups_;
  std::vector<size_t> detectors_;
  size_t coordinatesOffset_;
  size_t sourcesOffset_;
  size_t detectorsOffset_;
  size_t numCoordinates_;
  size_t numSources_;
  size_t numDetectors_;
};

class SystemConfiguration {
 public:
  SystemConfiguration(const YAML::Node& rootNode);

  const std::string& getConfigVersion() const {
    return configVersion_;
  }

  const std::string& getDeviceName() const {
    return deviceName_;
  }

  const std::vector<Canister>& getCanisters() const {
    return canisters_;
  }

  size_t getNumCoordinates() const {
    return numCoordinates_;
  }

  size_t getNumSources() const {
    return numSources_;
  }

  size_t getNumDetectors() const {
    return numDetectors_;
  }

 private:
  std::string configVersion_;
  std::string deviceName_;
  std::vector<Canister> canisters_;
  size_t numCoordinates_;
  size_t numSources_;
  size_t numDetectors_;
};

} // namespace configuration
} // namespace systems
} // namespace bci
