## Summary
The code snippet provides the implementation of functions for compressing and decompressing files and folders using the libzip library. It includes functions for compressing individual files, compressing folders, adding files and folders to a ZIP archive, and extracting files from a ZIP archive.

## Example Usage
```cpp
// Compress a file
std::string source_file = "path/to/source/file.txt";
std::string entry_name = "file.txt";
zip_t* zip_archive = zip_open("path/to/archive.zip", ZIP_CREATE | ZIP_TRUNCATE, nullptr);
ZipHelper::compressFile(source_file, zip_archive, entry_name);
zip_close(zip_archive);

// Compress a folder
std::string folder_path = "path/to/folder";
std::string zipfile_path = "path/to/archive.zip";
zip_archive = zip_open(zipfile_path.c_str(), ZIP_CREATE | ZIP_TRUNCATE, nullptr);
ZipHelper::compressFolder(folder_path, zip_archive, zipfile_path);
zip_close(zip_archive);

// Add a file or folder to a ZIP archive
std::string file_name = "path/to/file.txt";
std::string entry_name = "file.txt";
std::string zipfile_path = "path/to/archive.zip";
ZipHelper::addToZip(file_name, entry_name, zipfile_path);

// Extract files from a ZIP archive
std::string zipfile_path = "path/to/archive.zip";
std::string destination_path = "path/to/destination";
ZipHelper::unzipEntry(zipfile_path, destination_path);
```

## Code Analysis
### Inputs
- `source_file` (string): The path to the file to be compressed.
- `zip_archive` (zip_t*): Pointer to the ZIP archive.
- `entry_name` (string): The name of the entry in the ZIP archive.
- `folder_path` (string): The path to the folder to be compressed.
- `zipfile_path` (string): The path to the ZIP archive.
- `file_name` (string): The path to the file or folder to be added to the ZIP archive.
- `entry_name` (string): The name of the entry in the ZIP archive.
- `config` (ZipConfig): The configuration for compressing multiple files.
- `zipfile_path` (string): The path to the ZIP archive to be extracted.
- `destination_path` (string): The path to the destination directory for extracting files.
___
### Flow
1. The `compressFile` function compresses an individual file by opening the file, creating a zip source, adding the file to the ZIP archive, and closing the file and zip source.
2. The `compressFolder` function compresses a folder by checking if the folder exists and is a directory, adding the folder to the ZIP archive, and throwing an error if the folder cannot be added.
3. The `zipFile` function compresses a file by extracting the directory name, base name, and save name, and then calling the `compressFile` function.
4. The `zipDir` function compresses a directory by extracting the parent directory name, base name, and save name, and then calling the `compressFolder` function. It also recursively calls itself for subdirectories and calls `zipFile` for files.
5. The `addToZip` function adds a file or folder to the ZIP archive by checking if the ZIP archive is valid, and then calling `zipDir` or `zipFile` depending on the type of the input.
6. The `zipEntry` function compresses multiple files by opening the ZIP archive, iterating over the files in the configuration, and calling `addToZip` for each file. It then closes the ZIP archive.
7. The `unzipEntry` function extracts files from a ZIP archive by checking if the ZIP file exists, opening the ZIP archive, checking if the destination directory exists, and then iterating over the entries in the ZIP archive. For each entry, it checks if it is a directory or file, creates the directory if necessary, and extracts the file contents.
___
### Outputs
- None. The functions either complete successfully or throw a `ZipError` exception with an error message.
___
