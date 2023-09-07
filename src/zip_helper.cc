/*
 * ZipHelper namespace provides functions for compressing and decompressing
 * files and folders using the libzip library.
 *
 * Implementation of the functions declared in zip_helper.h.
 *
 * Author: Tianyun Xuan (tianyun.xuan@cn.innovusion.com)
 */
#include <zip_helper.h>

#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace ZipHelper {

void compressFile(const std::string& source_file, zip_t* zip_archive,
                  const std::string& entry_name) {
  if (zip_archive == nullptr) {
    throw ZipError("Invalid zip archive");
  }

  zip_source_t* source =
      zip_source_file(zip_archive, source_file.c_str(), 0, 0);
  if (!source) {
    throw ZipError("Failed to create zip source for file: " + source_file);
  }

  int zip_file_index =
      zip_file_add(zip_archive, entry_name.c_str(), source, ZIP_FL_ENC_UTF_8);
  if (zip_file_index < 0) {
    zip_source_free(source);
    throw ZipError("Failed to add file to ZIP archive: " + source_file +
                   " as " + entry_name);
  }

  zip_source_keep(source);
}

void compressFolder(const std::string& folder_path, zip_t* zip_archive,
                    const std::string& zipfile_path) {
  if (zip_archive == nullptr) {
    throw ZipError("Invalid zip archive");
  }

  if (!fs::exists(folder_path) || !fs::is_directory(folder_path)) {
    throw ZipError("Invalid folder path: " + folder_path);
  }

  if (!fs::is_empty(folder_path)) {
    int zip_dir_index =
        zip_dir_add(zip_archive, zipfile_path.c_str(), ZIP_FL_ENC_UTF_8);
    if (zip_dir_index < 0) {
      throw ZipError("Failed to add directory to ZIP archive: " + zipfile_path +
                     " as " + zipfile_path);
    }
  }
}

void zipFile(const std::string& file_name, const std::string& entry_name,
             zip_t* zip_archive) {
  const auto& dir_name = fs::path(file_name).parent_path().string();
  const auto& base_name = fs::path(file_name).filename().string();
  const std::string save_name =
      entry_name.size() ? entry_name + "/" + base_name : base_name;
  try {
    compressFile(file_name, zip_archive, save_name);
  } catch (const std::exception& e) {
    throw ZipError(e.what());
  }
}

void zipDir(const std::string& dir_name, const std::string& entry_name,
            zip_t* zip_archive, const bool root_dir) {
  const auto& parent_dir_name = fs::path(dir_name).parent_path().string();
  const auto& base_name = fs::path(dir_name).filename().string();
  const std::string save_name =
      root_dir ? entry_name
               : (entry_name.size() ? entry_name + "/" + base_name : base_name);
  try {
    compressFolder(dir_name, zip_archive, save_name);
    for (const auto& entry : fs::directory_iterator(dir_name)) {
      if (entry.is_directory()) {
        zipDir(entry.path().string(), save_name, zip_archive);
      } else {
        zipFile(entry.path().string(), save_name, zip_archive);
      }
    }
  } catch (const std::exception& e) {
    throw ZipError(e.what());
  }
}

void addToZip(const std::string& file_name, const std::string& entry_name,
              zip_t* zip_archive) {
  if (zip_archive == nullptr) {
    throw ZipError("Invalid zip archive");
  }
  try {
    if (fs::is_directory(file_name)) {
      zipDir(file_name, entry_name, zip_archive, true);
    } else {
      zipFile(file_name, entry_name, zip_archive);
    }
  } catch (const std::exception& e) {
    throw ZipError(e.what());
  }
}

void addToZip(const std::string& file_name, const std::string& entry_name,
              const std::string& zipfile_path) {
  if (!fs::exists(file_name)) {
    throw ZipError("Invalid file path: " + file_name);
  }

  zip_t* zip_archive =
      zip_open(zipfile_path.c_str(), ZIP_CREATE | ZIP_TRUNCATE, nullptr);
  if (!zip_archive) {
    throw ZipError("Failed to create ZIP archive: " + zipfile_path);
  }

  try {
    addToZip(file_name, entry_name, zip_archive);
  } catch (const std::exception& e) {
    zip_close(zip_archive);
    throw ZipError(e.what());
  }

  zip_close(zip_archive);
}

void zipEntry(const ZipConfig& config) {
  zip_t* zip_archive = zip_open(config.zip_destination.c_str(),
                                ZIP_CREATE | ZIP_TRUNCATE, nullptr);
  if (!zip_archive) {
    throw ZipError("Failed to create ZIP archive: " + config.zip_destination);
  }

  for (const auto& pair : config.files) {
    try {
      addToZip(pair.first, pair.second, zip_archive);
    } catch (const std::exception& e) {
      zip_close(zip_archive);
      throw ZipError(e.what());
    }
  }
  zip_close(zip_archive);
}

void unzipEntry(const std::string& zipfile_path,
                const std::string& destination_path) {
  constexpr int BUFFER_SIZE = 1024 * 1024;  // 1 MB buffer size

  // Check if the ZIP file exists
  if (!fs::exists(zipfile_path)) {
    throw ZipError("ZIP file not found: " + zipfile_path);
  }

  // Open the ZIP archive
  zip_t* zip_archive = zip_open(zipfile_path.c_str(), 0, nullptr);
  if (!zip_archive) {
    throw ZipError("Failed to open ZIP archive: " + zipfile_path);
  }

  // Get the number of entries in the ZIP archive
  int num_entries = zip_get_num_entries(zip_archive, 0);

  // Check if the ZIP file is empty
  if (num_entries == 0) {
    zip_close(zip_archive);
    return;  // ZIP file is empty, no need to extract anything
  }

  // Check if the destination directory exists
  if (!fs::exists(destination_path)) {
    if (!fs::create_directories(destination_path)) {
      zip_close(zip_archive);
      throw ZipError("Failed to create directory on disk: " + destination_path);
    }
  }

  // Extract each entry from the ZIP archive
  for (int i = 0; i < num_entries; ++i) {
    // Get the information about the current entry

    if (zip_archive == nullptr) {
      throw ZipError("Invalid zip_archive pointer.");
    }

    zip_stat_t entry_info;
    zip_stat_index(zip_archive, i, 0, &entry_info);
    if (entry_info.valid != 0x00FFu) {  // if not 255 (0x00FFu) then error
      throw ZipError("Failed to get information about entry in ZIP archive: " +
                     std::to_string(i));
    }

    // Check if the entry is a directory or file
    if (entry_info.name[strlen(entry_info.name) - 1] == '/') {
      const std::string dir_name = destination_path + "/" + entry_info.name;
      // Check if the destination directory exists
      if (!fs::exists(dir_name)) {
        if (!fs::create_directories(dir_name)) {
          zip_close(zip_archive);
          throw ZipError("Failed to create directory on disk: " + dir_name);
        }
      }
    } else {
      // Extract the file
      zip_file_t* file =
          zip_fopen_index(zip_archive, i, ZIP_FL_ENC_RAW | ZIP_FL_ENC_UTF_8);
      if (!file) {
        zip_close(zip_archive);
        throw ZipError("Failed to open file in ZIP archive: " +
                       std::string(entry_info.name));
      }

      if (entry_info.size < zip_uint64_t(BUFFER_SIZE)) {
        // Read and write the file contents
        std::vector<char> buf(entry_info.size);
        int bytesRead = zip_fread(file, buf.data(), buf.size());

        // Check if the read operation was successful
        if (bytesRead < 0) {
          zip_fclose(file);
          zip_close(zip_archive);
          throw ZipError("Failed to read file in ZIP archive: " +
                         std::string(entry_info.name));
        } else {
          // Create the file on disk
          std::ofstream outfile(destination_path + "/" + entry_info.name,
                                std::ios::binary);
          if (!outfile) {
            zip_fclose(file);
            zip_close(zip_archive);
            throw ZipError("Failed to create file on disk: " +
                           std::string(entry_info.name));
          }
          outfile.write(buf.data(), bytesRead);
          outfile.close();

          // Clean up
          zip_fclose(file);
        }

      } else {
        // Create the file on disk
        std::ofstream outfile(destination_path + "/" + entry_info.name,
                              std::ios::binary);
        if (!outfile) {
          zip_fclose(file);
          zip_close(zip_archive);
          throw ZipError("Failed to create file on disk: " +
                         std::string(entry_info.name));
        }

        // Read and write the file contents in chunks
        std::vector<char> buf(BUFFER_SIZE);
        int bytesRead = 0;
        while (zip_uint64_t(bytesRead) < entry_info.size) {
          int bytesToRead = std::min(
              BUFFER_SIZE, static_cast<int>(entry_info.size - bytesRead));

          bytesRead += zip_fread(file, buf.data(), bytesToRead);

          // Check if the read operation was successful
          if (bytesRead == 0) {
            zip_fclose(file);
            zip_close(zip_archive);
            throw ZipError("Failed to read file in ZIP archive: " +
                           std::string(entry_info.name));
          } else {
            // Write the file contents to disk
            outfile.write(buf.data(), bytesRead);
          }
        }

        // Close the file
        outfile.close();
        // Clean up
        zip_fclose(file);
      }
    }
  }

  // Close the ZIP archive
  zip_close(zip_archive);
}

}  // namespace ZipHelper
