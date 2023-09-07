cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(zip_helper)

if(ENABLE_COVERAGE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
SET(CMAKE_SKIP_BUILD_RPATH FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(PACKAGE_VERSION 1.0)

# set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -lfmt -fopenmp -Wall -pthread")
if(ENABLE_COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -coverage")
  set(ENABLE_UNITTEST True)
endif()
# if(ENABLE_LEAK)
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fsanitize=address -fsanitize-recover=all  -fsanitize=leak")
# endif()

find_package(LibZip REQUIRED)

if(ENABLE_UNITTEST)
  find_package(GTest REQUIRED)
  enable_testing()
endif()

aux_source_directory(./src/ DIR_ZIP_HELPER_SRCS)
add_library(zip_helper SHARED ${DIR_ZIP_HELPER_SRCS})
target_link_libraries(zip_helper -lzip)
target_include_directories(zip_helper PUBLIC include)

## Install library ##
install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION lib
)

## Install headers ##
install(DIRECTORY include
        DESTINATION include/${PROJECT_NAME})

## Unit test ##
if(ENABLE_UNITTEST)
        add_executable(zip_helper_test test/zip_helper_test.cc)
        target_link_libraries(zip_helper_test zip_helper gtest gtest_main)
        gtest_discover_tests(zip_helper_test)
endif()