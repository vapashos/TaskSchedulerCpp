cmake_minimum_required(VERSION 3.30)
project("TaskScheduleApp" LANGUAGES CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (SRC_FOLDER "src")
set (LIB_FOLDER "lib")
set (TASK_SCHEDULER_LIB         "TaskSchedulerLib.lib")
set (TASK_SCHEDULER_DLL         "TaskSchedulerLib.dll")
set (TASK_SCHEDULER_INCLUDES    ${CMAKE_CURRENT_SOURCE_DIR}/${SRC_FOLDER}/${LIB_FOLDER})
set (CMAKE_INSTALL_PREFIX       ${CMAKE_CURRENT_SOURCE_DIR}/install)
# Enable position-independent code (PIC) for shared libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Detect the platform and set platform-specific options
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "Building on Windows")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Building on Linux")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Building on macOS")
    set(CMAKE_MACOSX_RPATH 1) # Ensure RPATH is correctly set on macOS
endif()

message(STATUS "CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}")

# Add subdirectories
add_subdirectory(${SRC_FOLDER}/lib)   
add_subdirectory(${SRC_FOLDER}/app)