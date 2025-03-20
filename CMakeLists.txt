cmake_minimum_required(VERSION 3.21)
project("TaskScheduleApp" LANGUAGES CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (SRC_FOLDER "src")
set (LIB_FOLDER "lib")
set (CMAKE_INSTALL_PREFIX          ${CMAKE_CURRENT_SOURCE_DIR}/install)
set (TASK_SCHEDULER_LIB_BINARIES   ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)
# Enable position-independent code (PIC) for shared libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set (TASK_SCHEDULER_LIB_NAME   "TaskSchedulerLib")
# Detect the platform and set platform-specific options
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Building on Windows")
    set (TASK_SCHEDULER_LIB_TARGET  "${TASK_SCHEDULER_LIB_NAME}.lib")
    set (TASK_SCHEDULER_LIB_BINARY  "${TASK_SCHEDULER_LIB_NAME}.lib")
    set (TASK_SCHEDULER_BIN_NAME    "${TASK_SCHEDULER_LIB_NAME}.dll")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Building on Linux")
    message(STATUS TASK_SCHEDULER_LIB_BINARIES = ${TASK_SCHEDULER_LIB_BINARIES})
    set (TASK_SCHEDULER_LIB_TARGET  "lib${TASK_SCHEDULER_LIB_NAME}.so")
    set (TASK_SCHEDULER_LIB_BINARY  "${TASK_SCHEDULER_LIB_NAME}.so")
    set (TASK_SCHEDULER_BIN_NAME    "${TASK_SCHEDULER_LIB_NAME}.so")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Building on macOS")
    set(CMAKE_MACOSX_RPATH 1) # Ensure RPATH is correctly set on macOS
endif()

message(STATUS "CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}")

# Add subdirectories
add_subdirectory(${SRC_FOLDER}/lib)   
add_subdirectory(${SRC_FOLDER}/app)