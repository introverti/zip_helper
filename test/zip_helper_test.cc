/*
 * ZipHelper namespace provides functions for compressing and decompressing
 * files and folders using the libzip library.
 *
 * Unit tests for the functions declared in zip_helper.h.
 *
 * Author: Tianyun Xuan (tianyun.xuan@cn.innovusion.com)
 */
#include "zip_helper.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "gtest/gtest.h"

using namespace ZipHelper;
namespace fs = std::filesystem;

// 定义一个测试夹具类，用于设置和清理测试环境
class ZipHelperTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // 在测试之前进行一些初始化工作，如果需要的话
  }

  void TearDown() override {
    // 在测试之后进行一些清理工作，如果需要的话
  }
};

TEST_F(ZipHelperTest, PreapareTestData) {
  ASSERT_TRUE(fs::create_directory("test_main_directory"));
  ASSERT_TRUE(fs::create_directory("test_main_directory/test_sub_directory"));
  ASSERT_TRUE(fs::create_directory("unzip_directory"));
  std::ofstream file1("test_main_directory/file1.txt");
  ASSERT_TRUE(file1.is_open());
  file1 << "This is file1";
  file1.close();
  std::ofstream file2("test_main_directory/test_sub_directory/file2.txt");
  ASSERT_TRUE(file2.is_open());
  file2 << "This is file2";
  file2.close();
}

// Test case for the Zip function
TEST_F(ZipHelperTest, ZipTest) {
  // Create a ZipConfig object with test data
  ZipConfig config{.files = {{"test_main_directory", "zipped_main_directory"}},
                   .zip_destination = "test_zip.zip"};

  // Call the Zip function
  ASSERT_NO_THROW(zipEntry(config));
  // Add assertions to check if the zip file is created successfully or any
  // other specific conditions
}

// Test case for the UnZip function
TEST_F(ZipHelperTest, UnZipTest) {
  std::string zipfile_path = "test_zip.zip";
  std::string destination_path = "unzip_directory";

  // Call the UnZip function
  ASSERT_NO_THROW(unzipEntry(zipfile_path, destination_path));
  // Add assertions to check if the files are extracted successfully or any
  // other specific conditions
}

TEST_F(ZipHelperTest, CheckData) {
  std::ifstream file1("unzip_directory/zipped_main_directory/file1.txt");
  ASSERT_TRUE(file1.is_open());
  std::string line1;
  std::getline(file1, line1);
  ASSERT_EQ(line1, "This is file1");
  file1.close();
  std::ifstream file2(
      "unzip_directory/zipped_main_directory/test_sub_directory/file2.txt");
  ASSERT_TRUE(file2.is_open());
  std::string line2;
  std::getline(file2, line2);
  ASSERT_EQ(line2, "This is file2");
  file2.close();
}

TEST_F(ZipHelperTest, CleanUpTestData) {
  ASSERT_TRUE(fs::remove_all("test_main_directory"));
  ASSERT_TRUE(fs::remove_all("unzip_directory"));
  ASSERT_TRUE(fs::remove("test_zip.zip"));
}