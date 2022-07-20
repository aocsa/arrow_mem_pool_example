cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(arrow-memory-pool-test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FeatureSummary)
include(CheckIncludeFiles)
include(CheckLibraryExists)

set(ARROW_HOME $ENV{CONDA_PREFIX})
set(CMAKE_SYSTEM_PREFIX_PATH
  "${ARROW_HOME};${CMAKE_SYSTEM_PREFIX_PATH}"
)

find_package(Arrow REQUIRED)

include_directories(
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/include
  ${ARROW_INCLUDE_DIR}
)

link_directories(${ARROW_LIB_DIR})

set(SRC_FILES
  ${PROJECT_SOURCE_DIR}/main.cpp
)

add_executable(arrow-memory-pool-test ${SRC_FILES})
target_link_libraries(arrow-memory-pool-test
  libarrow.so
  libarrow_dataset.so
  libparquet.so
  libarrow_testing.so
  libgtest.so
)

feature_summary(
  WHAT ALL
  INCLUDE_QUIET_PACKAGES
  FATAL_ON_MISSING_REQUIRED_PACKAGES
)
