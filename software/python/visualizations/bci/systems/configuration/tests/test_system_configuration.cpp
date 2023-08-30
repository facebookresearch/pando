// Copyright 2004-present Facebook. All Rights Reserved.
// System configuration tests

#include "arvr/bci/systems/configuration/system_configuration.h"

#include <array>
#include <fstream>
#include <streambuf>
#include <string>
#include <unordered_map>
#include <utility>

#include <gtest/gtest.h>

constexpr double kTestMaxCoordinateError = 1e-9;

constexpr char kTestFilename[] = "arvr/bci/systems/configuration/tests/test_configuration.yaml";
constexpr char kTestName[] = "test";
constexpr char kTestConfigVersion[] = "1.1.0";
constexpr size_t kTestNumCanisters = 3;
constexpr size_t kTestNumSourceGroups[3] = {2, 1, 1};
constexpr size_t kTestSourceGroupNumSources[4] = {9, 8, 4, 4};
constexpr size_t kTestCanistersNumCoordinates[3] = {8, 4, 4};
constexpr size_t kTestCanistersNumSources[3] = {
    kTestSourceGroupNumSources[0] + kTestSourceGroupNumSources[1],
    kTestSourceGroupNumSources[2],
    kTestSourceGroupNumSources[3],
};
constexpr size_t kTestCanistersNumDetectors[3] = {8, 4, 4};
constexpr size_t kTestNumCoordinates = kTestCanistersNumCoordinates[0] +
    kTestCanistersNumCoordinates[1] + kTestCanistersNumCoordinates[2];
constexpr size_t kTestNumSources =
    kTestCanistersNumSources[0] + kTestCanistersNumSources[1] + kTestCanistersNumSources[2];
constexpr size_t kTestNumDetectors =
    kTestCanistersNumDetectors[0] + kTestCanistersNumDetectors[1] + kTestCanistersNumDetectors[2];

constexpr double kTestCoordinates0[3][3] = {{0.0, 0.0, 0.0}, {100.0, 0.0, 0.0}, {100.0, 0.0, 0.0}};
constexpr double kTestCoordinates1[3][3] = {{1.0, 0.0, 0.0}, {100.0, 1.0, 0.0}, {100.0, 1.0, 0.0}};
constexpr double kTestCoordinates2[3][3] = {{0.0, 1.0, 0.0}, {99.0, 0.0, 0.0}, {99.0, 0.0, 0.0}};

constexpr size_t kTestSources0[4] = {0, 3, 0, 0};
constexpr size_t kTestSources1[4] = {1, 2, 1, 1};
constexpr size_t kTestSources2[4] = {2, 1, 2, 2};

constexpr size_t kTestDetectors0[3] = {0, 0, 0};
constexpr size_t kTestDetectors1[3] = {1, 1, 1};
constexpr size_t kTestDetectors2[3] = {2, 2, 2};

constexpr char kSimmonsFilename[] = "arvr/bci/systems/simmons/configuration/motor-left-dorsal.yaml";
constexpr char kSimmonsName[] = "simmons";
constexpr char kSimmonsConfigVersion[] = "1.1.0";
constexpr size_t kSimmonsNumCanisters = 1;
constexpr size_t kSimmonsNumCoordinates = 80;
constexpr size_t kSimmonsNumSources = 82;
constexpr size_t kSimmonsNumDetectors = 39;
constexpr char kSimmonsCanisterName[] = "MotorLeftDorsal";
constexpr size_t kSimmonsCanisterNumCoordinates = 80;
constexpr size_t kSimmonsCanisterNumSources = 82;
constexpr size_t kSimmonsCanisterNumDetectors = 39;
constexpr std::array<std::pair<const char*, double>, 2> kSimmonsCanisterWavelengths = {{
    {"Red", 680.0},
    {"Infrared", 850.0},
}};
constexpr size_t kSimmonsNumSourceGroups = 2;
constexpr char kSimmonsSourceGroupRedType[] = "Red";
constexpr size_t kSimmonsSourceGroupNumRedSources = 41;
constexpr char kSimmonsSourceGroupInfraredType[] = "Infrared";
constexpr size_t kSimmonsSourceGroupNumInfraredSources = 41;

constexpr std::array<double, 3> kSimmonsCoordinate0 = {-28.15, 13, 0};
constexpr std::array<double, 3> kSimmonsCoordinate1 = {-28.15, 6.5, 0};
constexpr std::array<double, 3> kSimmonsCoordinate2 = {-28.15, 0, 0};

constexpr size_t kSimmonsRedSource0 = 26;
constexpr size_t kSimmonsRedSource1 = 35;
constexpr size_t kSimmonsRedSource2 = 45;

constexpr size_t kSimmonsInfraredSource0 = 26;
constexpr size_t kSimmonsInfraredSource1 = 35;
constexpr size_t kSimmonsInfraredSource2 = 45;

constexpr size_t kSimmonsDetector0 = 11;
constexpr size_t kSimmonsDetector1 = 18;
constexpr size_t kSimmonsDetector2 = 27;

TEST(SystemConfigurationTest, ConfigurationTest) {
  const std::vector<std::string> kTestSourceGroupTypes = {"Red", "Infrared", "Red", "Red"};
  const std::vector<std::string> kTestCanistersName = {"Test0", "Test1", "Test2"};
  const std::vector<std::vector<std::pair<std::string, double>>> kTestWavelengths = {
      {{"Red", 50.0}, {"Infrared", 100.0}}, {{"Red", 50.0}}, {{"Red", 50.0}}};

  std::fstream yamlFile(kTestFilename, std::fstream::in);

  std::string fileString;
  yamlFile.seekg(0, std::ios::end);
  fileString.reserve(yamlFile.tellg());
  yamlFile.seekg(0, std::ios::beg);

  fileString.assign(std::istreambuf_iterator<char>(yamlFile), std::istreambuf_iterator<char>());

  YAML::Node rootNode = YAML::Load(fileString);
  bci::systems::configuration::SystemConfiguration systemConfiguration(rootNode);

  EXPECT_EQ(systemConfiguration.getConfigVersion(), kTestConfigVersion);
  EXPECT_EQ(systemConfiguration.getDeviceName(), kTestName);
  EXPECT_EQ(systemConfiguration.getNumCoordinates(), kTestNumCoordinates);
  EXPECT_EQ(systemConfiguration.getNumSources(), kTestNumSources);
  EXPECT_EQ(systemConfiguration.getNumDetectors(), kTestNumDetectors);

  const std::vector<bci::systems::configuration::Canister>& canisters =
      systemConfiguration.getCanisters();
  ASSERT_EQ(canisters.size(), kTestNumCanisters);

  size_t sourceGroupOffset = 0;
  for (size_t c = 0; c < canisters.size(); ++c) {
    const std::map<std::string, bci::systems::configuration::SourceGroup>& sourceGroups =
        canisters[c].getSourceGroups();

    EXPECT_EQ(canisters[c].getCanisterName(), kTestCanistersName[c]);

    const std::unordered_map<std::string, double>& wavelengths = canisters[c].getWavelengths();
    ASSERT_EQ(wavelengths.size(), kTestWavelengths[c].size());
    for (size_t i = 0; i < kTestWavelengths[c].size(); ++i) {
      const auto& [key, value] = kTestWavelengths[c][i];
      EXPECT_EQ(1, wavelengths.count(key));
      EXPECT_EQ(value, wavelengths.at(key));
    }

    EXPECT_EQ(canisters[c].getNumCoordinates(), kTestCanistersNumCoordinates[c]);
    EXPECT_EQ(canisters[c].getNumSources(), kTestCanistersNumSources[c]);
    EXPECT_EQ(canisters[c].getNumDetectors(), kTestCanistersNumDetectors[c]);

    ASSERT_EQ(sourceGroups.size(), kTestNumSourceGroups[c]);

    EXPECT_NEAR(
        canisters[c].getCoordinates()[0][0], kTestCoordinates0[c][0], kTestMaxCoordinateError);
    EXPECT_NEAR(
        canisters[c].getCoordinates()[0][1], kTestCoordinates0[c][1], kTestMaxCoordinateError);
    EXPECT_NEAR(
        canisters[c].getCoordinates()[0][2], kTestCoordinates0[c][2], kTestMaxCoordinateError);
    EXPECT_NEAR(
        canisters[c].getCoordinates()[1][0], kTestCoordinates1[c][0], kTestMaxCoordinateError);
    EXPECT_NEAR(
        canisters[c].getCoordinates()[1][1], kTestCoordinates1[c][1], kTestMaxCoordinateError);
    EXPECT_NEAR(
        canisters[c].getCoordinates()[1][2], kTestCoordinates1[c][2], kTestMaxCoordinateError);
    EXPECT_NEAR(
        canisters[c].getCoordinates()[2][0], kTestCoordinates2[c][0], kTestMaxCoordinateError);
    EXPECT_NEAR(
        canisters[c].getCoordinates()[2][1], kTestCoordinates2[c][1], kTestMaxCoordinateError);
    EXPECT_NEAR(
        canisters[c].getCoordinates()[2][2], kTestCoordinates2[c][2], kTestMaxCoordinateError);

    EXPECT_EQ(canisters[c].getDetectors()[0], kTestDetectors0[c]);
    EXPECT_EQ(canisters[c].getDetectors()[1], kTestDetectors1[c]);
    EXPECT_EQ(canisters[c].getDetectors()[2], kTestDetectors2[c]);

    for (size_t g = 0; g < kTestNumSourceGroups[c]; ++g) {
      size_t totalGroupIndex = sourceGroupOffset + g;
      const auto& iterator = sourceGroups.find(kTestSourceGroupTypes[totalGroupIndex].c_str());
      ASSERT_EQ(iterator != sourceGroups.end(), true);
      const auto& group = iterator->second;

      EXPECT_EQ(group.getWavelength(), kTestWavelengths[c][g].second);

      EXPECT_EQ(group.getNumSources(), kTestSourceGroupNumSources[totalGroupIndex]);

      EXPECT_EQ(group.getSources()[0], kTestSources0[totalGroupIndex]);
      EXPECT_EQ(group.getSources()[1], kTestSources1[totalGroupIndex]);
      EXPECT_EQ(group.getSources()[2], kTestSources2[totalGroupIndex]);
    }
    sourceGroupOffset += kTestNumSourceGroups[c];
  }
}

TEST(SystemConfigurationTest, SimmonsConfigurationTest) {
  std::fstream yamlFile(kSimmonsFilename, std::fstream::in);

  std::string fileString;
  yamlFile.seekg(0, std::ios::end);
  fileString.reserve(yamlFile.tellg());
  yamlFile.seekg(0, std::ios::beg);

  fileString.assign(std::istreambuf_iterator<char>(yamlFile), std::istreambuf_iterator<char>());

  YAML::Node rootNode = YAML::Load(fileString);
  bci::systems::configuration::SystemConfiguration systemConfiguration(rootNode);
  const std::vector<bci::systems::configuration::Canister>& canisters =
      systemConfiguration.getCanisters();
  const std::map<std::string, bci::systems::configuration::SourceGroup>& sourceGroups =
      canisters[0].getSourceGroups();

  EXPECT_EQ(systemConfiguration.getConfigVersion(), kSimmonsConfigVersion);
  EXPECT_EQ(systemConfiguration.getDeviceName(), kSimmonsName);
  EXPECT_EQ(systemConfiguration.getNumCoordinates(), kSimmonsNumCoordinates);
  EXPECT_EQ(systemConfiguration.getNumSources(), kSimmonsNumSources);
  EXPECT_EQ(systemConfiguration.getNumDetectors(), kSimmonsNumDetectors);

  ASSERT_EQ(canisters.size(), kSimmonsNumCanisters);
  EXPECT_EQ(canisters[0].getCanisterName(), kSimmonsCanisterName);
  EXPECT_EQ(canisters[0].getNumCoordinates(), kSimmonsCanisterNumCoordinates);
  EXPECT_EQ(canisters[0].getNumSources(), kSimmonsCanisterNumSources);
  EXPECT_EQ(canisters[0].getNumDetectors(), kSimmonsCanisterNumDetectors);

  const std::unordered_map<std::string, double>& wavelengths = canisters[0].getWavelengths();
  ASSERT_EQ(wavelengths.size(), kSimmonsCanisterWavelengths.size());
  for (size_t i = 0; i < kSimmonsCanisterWavelengths.size(); ++i) {
    const auto& [key, value] = kSimmonsCanisterWavelengths[i];
    EXPECT_EQ(1, wavelengths.count(key));
    EXPECT_EQ(value, wavelengths.at(key));
  }

  ASSERT_EQ(sourceGroups.size(), kSimmonsNumSourceGroups);
  ASSERT_EQ(sourceGroups.find(kSimmonsSourceGroupRedType) != sourceGroups.end(), true);
  const auto& redGroup = sourceGroups.find(kSimmonsSourceGroupRedType)->second;
  ASSERT_EQ(sourceGroups.find(kSimmonsSourceGroupInfraredType) != sourceGroups.end(), true);
  const auto& infraredGroup = sourceGroups.find(kSimmonsSourceGroupInfraredType)->second;

  EXPECT_EQ(redGroup.getWavelength(), kSimmonsCanisterWavelengths[0].second);
  EXPECT_EQ(infraredGroup.getWavelength(), kSimmonsCanisterWavelengths[1].second);

  EXPECT_EQ(redGroup.getNumSources(), kSimmonsSourceGroupNumRedSources);
  EXPECT_EQ(infraredGroup.getNumSources(), kSimmonsSourceGroupNumInfraredSources);

  EXPECT_EQ(canisters[0].getCoordinates()[0], kSimmonsCoordinate0);
  EXPECT_EQ(canisters[0].getCoordinates()[1], kSimmonsCoordinate1);
  EXPECT_EQ(canisters[0].getCoordinates()[2], kSimmonsCoordinate2);

  EXPECT_EQ(redGroup.getSources()[0], kSimmonsRedSource0);
  EXPECT_EQ(redGroup.getSources()[1], kSimmonsRedSource1);
  EXPECT_EQ(redGroup.getSources()[2], kSimmonsRedSource2);

  EXPECT_EQ(infraredGroup.getSources()[0], kSimmonsInfraredSource0);
  EXPECT_EQ(infraredGroup.getSources()[1], kSimmonsInfraredSource1);
  EXPECT_EQ(infraredGroup.getSources()[2], kSimmonsInfraredSource2);

  EXPECT_EQ(canisters[0].getDetectors()[0], kSimmonsDetector0);
  EXPECT_EQ(canisters[0].getDetectors()[1], kSimmonsDetector1);
  EXPECT_EQ(canisters[0].getDetectors()[2], kSimmonsDetector2);
}
