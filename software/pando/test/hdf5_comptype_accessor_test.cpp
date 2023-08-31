#include "hdf5_comptype_accessor.h"

#include <H5Cpp.h>

#include "gtest/gtest.h"

#include <cstdint>

namespace pnd {
namespace pando {

/** Test that HDF5 compound type fields are all the same size as the arithmetic types we use to
 * represent them. */
TEST(HDF5CompTypeFieldTest, FieldSize) {
  using FD = HDF5CompTypeFieldDescriptor;
  EXPECT_EQ(FD::TypeTraits<FD::CPPTYPE_INT8>::size, FD::GetPredType(FD::CPPTYPE_INT8).getSize());
  EXPECT_EQ(FD::TypeTraits<FD::CPPTYPE_INT16>::size, FD::GetPredType(FD::CPPTYPE_INT16).getSize());
  EXPECT_EQ(FD::TypeTraits<FD::CPPTYPE_INT32>::size, FD::GetPredType(FD::CPPTYPE_INT32).getSize());
  EXPECT_EQ(FD::TypeTraits<FD::CPPTYPE_INT64>::size, FD::GetPredType(FD::CPPTYPE_INT64).getSize());
  EXPECT_EQ(FD::TypeTraits<FD::CPPTYPE_UINT8>::size, FD::GetPredType(FD::CPPTYPE_UINT8).getSize());
  EXPECT_EQ(
      FD::TypeTraits<FD::CPPTYPE_UINT16>::size, FD::GetPredType(FD::CPPTYPE_UINT16).getSize());
  EXPECT_EQ(
      FD::TypeTraits<FD::CPPTYPE_UINT32>::size, FD::GetPredType(FD::CPPTYPE_UINT32).getSize());
  EXPECT_EQ(
      FD::TypeTraits<FD::CPPTYPE_UINT64>::size, FD::GetPredType(FD::CPPTYPE_UINT64).getSize());
  EXPECT_EQ(FD::TypeTraits<FD::CPPTYPE_FLOAT>::size, FD::GetPredType(FD::CPPTYPE_FLOAT).getSize());
  EXPECT_EQ(
      FD::TypeTraits<FD::CPPTYPE_DOUBLE>::size, FD::GetPredType(FD::CPPTYPE_DOUBLE).getSize());
}

class HDF5CompTypeAccessorTest : public ::testing::Test {
 protected:
  struct CompTypeStruct {
    int32_t i;
    float f;
    uint8_t s[4];
  };

  HDF5CompTypeAccessorTest() {
    // Buid an HDF5 compound datatype with the same fields as CompTypeStruct
    comp_type_.insertMember("i", HOFFSET(CompTypeStruct, i), H5::PredType::NATIVE_INT32);
    comp_type_.insertMember("f", HOFFSET(CompTypeStruct, f), H5::PredType::NATIVE_FLOAT);
    hsize_t dim = sizeof(CompTypeStruct::s);
    H5::ArrayType arr_type(H5::PredType::NATIVE_UINT8, 1, &dim);
    comp_type_.insertMember("s", HOFFSET(CompTypeStruct, s), arr_type);
    accessor_ = HDF5CompTypeAccessor{comp_type_};
  }

  H5::CompType comp_type_{sizeof(CompTypeStruct)};
  HDF5CompTypeAccessor accessor_;
};

// Test that the H5::CompType generating constructor generates a type identical to the one we built
// by hand in the test case.
TEST_F(HDF5CompTypeAccessorTest, SpecifierConstructor) {
  HDF5CompTypeAccessor::CompTypeSpecifier comptype_specifier;

  comptype_specifier.AddField("i", HDF5CompTypeFieldDescriptor::CPPTYPE_INT32);
  comptype_specifier.AddField("f", HDF5CompTypeFieldDescriptor::CPPTYPE_FLOAT);
  comptype_specifier.AddField("s", HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8, 4);

  HDF5CompTypeAccessor type_building_accessor{comptype_specifier};

  EXPECT_EQ(type_building_accessor.GetH5Type(), accessor_.GetH5Type());
  EXPECT_EQ(type_building_accessor.GetH5TypeSize(), accessor_.GetH5TypeSize());
}

// Check getting descriptor for the integer field
TEST_F(HDF5CompTypeAccessorTest, Int32FieldDesc) {
  auto fd = accessor_.GetFieldDescriptor("i");
  ASSERT_TRUE(fd);
  EXPECT_EQ(fd->GetCppType(), HDF5CompTypeFieldDescriptor::CPPTYPE_INT32);
  EXPECT_EQ(fd->GetArrayLen(), 1);
}

// Check getting descriptor for the float field
TEST_F(HDF5CompTypeAccessorTest, FloatFieldDesc) {
  auto fd = accessor_.GetFieldDescriptor("f");
  ASSERT_TRUE(fd);
  EXPECT_EQ(fd->GetCppType(), HDF5CompTypeFieldDescriptor::CPPTYPE_FLOAT);
  EXPECT_EQ(fd->GetArrayLen(), 1);
}

// Check getting descriptor for the array field
TEST_F(HDF5CompTypeAccessorTest, ArrayFieldDesc) {
  auto fd = accessor_.GetFieldDescriptor("s");
  ASSERT_TRUE(fd);
  EXPECT_EQ(fd->GetCppType(), HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8);
  EXPECT_EQ(fd->GetArrayLen(), sizeof(CompTypeStruct::s));
}

// Check FieldDesc==nullptr when the field name doesn't exist.
TEST_F(HDF5CompTypeAccessorTest, NonexistantFieldDesc) {
  auto fd = accessor_.GetFieldDescriptor("nonexistant");
  EXPECT_FALSE(fd);
}

// Check Extraction of fields from accessor_ binary blob
TEST_F(HDF5CompTypeAccessorTest, ExtractAll) {
  // An array of 2 CompTypeStructs with accessor_ unique value in every field
  CompTypeStruct test_structs[2]{{1, 2.0f, {3, 4, 5, 6}}, {7, 8.0f, {9, 10, 11}}};

  // Copy test_structs into accessor_ vector of bytes
  // This vector is analogous to what pando::HDF5Table produces during accessor_ read of 2 cells.

  auto data_p = reinterpret_cast<uint8_t*>(&test_structs);
  std::vector<uint8_t> buf{data_p, data_p + sizeof(test_structs)};

  auto i_fd = accessor_.GetFieldDescriptor("i");
  auto f_fd = accessor_.GetFieldDescriptor("f");
  auto s_fd = accessor_.GetFieldDescriptor("s");

  ASSERT_TRUE(i_fd);
  ASSERT_TRUE(f_fd);
  ASSERT_TRUE(s_fd);

  // Read back values from first CompTypeStruct
  EXPECT_EQ(accessor_.GetField<HDF5CompTypeFieldDescriptor::CPPTYPE_INT32>(buf, i_fd, 0), 1);
  EXPECT_EQ(accessor_.GetField<HDF5CompTypeFieldDescriptor::CPPTYPE_FLOAT>(buf, f_fd, 0), 2.0f);
  EXPECT_EQ(accessor_.GetFieldP<HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8>(buf, s_fd, 0)[0], 3);
  EXPECT_EQ(accessor_.GetFieldP<HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8>(buf, s_fd, 0)[1], 4);
  EXPECT_EQ(accessor_.GetFieldP<HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8>(buf, s_fd, 0)[2], 5);
  EXPECT_EQ(accessor_.GetFieldP<HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8>(buf, s_fd, 0)[3], 6);

  // Read back values from second CompTypeStruct
  EXPECT_EQ(accessor_.GetField<HDF5CompTypeFieldDescriptor::CPPTYPE_INT32>(buf, i_fd, 1), 7);
  EXPECT_EQ(accessor_.GetField<HDF5CompTypeFieldDescriptor::CPPTYPE_FLOAT>(buf, f_fd, 1), 8.0f);
  EXPECT_EQ(accessor_.GetFieldP<HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8>(buf, s_fd, 1)[0], 9);
  EXPECT_EQ(accessor_.GetFieldP<HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8>(buf, s_fd, 1)[1], 10);
  EXPECT_EQ(accessor_.GetFieldP<HDF5CompTypeFieldDescriptor::CPPTYPE_UINT8>(buf, s_fd, 1)[2], 11);
}

} // namespace pando
} // namespace pnd
