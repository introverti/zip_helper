/*
 * ZipHelper namespace provides functions for compressing and decompressing
 * files and folders using the libzip library.
 *
 * Functions:
 * - compressFile: Compresses a file and adds it to the ZIP archive.
 * - compressFolder: Compresses a folder and adds it to the ZIP archive.
 * - zipFile: Compresses a file by calling compressFile function.
 * - zipDir: Compresses a folder by calling compressFolder function. It also
 * recursively calls itself for each subdirectory and calls the zipFile function
 * for each file.
 * - addToZip: Adds a file or folder to the ZIP archive by checking if it is a
 * directory and calling the zipDir function, or calling the zipFile function.
 * - zipEntry: Creates a ZIP archive and adds multiple files or folders to it by
 * iterating over the config.files map and calling the addToZip function for
 * each pair.
 * - unzipEntry: Extracts files from a ZIP archive by opening the archive,
 * checking if it is empty, creating the destination directory if it doesn't
 * exist, and then iterating over each entry in the archive. For each entry, it
 * checks if it is a directory and creates the directory if it doesn't exist, or
 * extracts the file by reading and writing its contents.
 *
 * Exceptions:
 * - ZipError: Thrown if there is an error during compression or extraction.
 *
 * Author: Tianyun Xuan (tianyun.xuan@cn.innovusion.com)
 */
#pragma once
#include <zip.h>

#include <cstring>
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>

namespace ZipHelper {

/**
 * @brief Custom exception class for ZipHelper errors.
 */
class ZipError : public std::runtime_error {
 public:
  /**
   * @brief Constructs a ZipError object with the specified error message.
   * @param _arg The error message.
   */
  explicit ZipError(const std::string& _arg) noexcept
      : std::runtime_error(_arg) {}
};

/**
 * @brief Structure to hold the configuration for zipping files and folders.
 */
struct ZipConfig {
  std::vector<std::pair<std::string, std::string>> files;  // files or folders
  std::string zip_destination;                             // "zip/0001.zip"
};

/**
 * @brief Zips the files and folders specified in the ZipConfig object.
 * @param config The ZipConfig object containing the files and folders to be
 * zipped.
 */
void zipEntry(const ZipConfig& config);

/**
 * @brief Adds a file to the zip archive.
 * @param file_path The path of the file to be added.
 * @param entry_path The path of the file entry in the zip archive.
 * @param zipfile_path The path of the zip archive.
 */
void addToZip(const std::string& file_path, const std::string& entry_path,
              const std::string& zipfile_path);

/**
 * @brief Adds a file to the zip archive.
 * @param file_name The name of the file to be added.
 * @param entry_name The name of the file entry in the zip archive.
 * @param zip_archive The zip archive.
 */
void addToZip(const std::string& file_name, const std::string& entry_name,
              zip_t* zip_archive);

/**
 * @brief Zips a single file.
 * @param file_name The name of the file to be zipped.
 * @param entry_name The name of the file entry in the zip archive.
 * @param zip_archive The zip archive.
 */
void zipFile(const std::string& file_name, const std::string& entry_name,
             zip_t* zip_archive);

/**
 * @brief Zips a directory and its contents.
 * @param dir_name The name of the directory to be zipped.
 * @param entry_name The name of the directory entry in the zip archive.
 * @param zip_archive The zip archive.
 * @param root_dir Flag indicating whether the root directory should be included
 * in the zip archive.
 */
void zipDir(const std::string& dir_name, const std::string& entry_name,
            zip_t* zip_archive, const bool root_dir = false);

/**
 * @brief Compresses a file and adds it to the zip archive.
 * @param source_file The path of the file to be compressed.
 * @param zip_archive The zip archive.
 * @param entry_name The name of the file entry in the zip archive.
 */
void compressFile(const std::string& source_file, zip_t* zip_archive,
                  const std::string& entry_name);

/**
 * @brief Compresses a folder and adds its contents to the zip archive.
 * @param folder_path The path of the folder to be compressed.
 * @param zip_archive The zip archive.
 * @param zipfile_path The path of the zip archive.
 */
void compressFolder(const std::string& folder_path, zip_t* zip_archive,
                    const std::string& zipfile_path);

/**
 * @brief Unzips a file or folder from the specified zip archive.
 * @param zipfile_path The path of the zip archive.
 * @param destination_path The path where the unzipped file or folder should be
 * placed.
 */
void unzipEntry(const std::string& zipfile_path,
                const std::string& destination_path);

}  // namespace ZipHelper
