# CMakeList.txt : CMake project for TaskScheduler, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.21)
project(TaskSchedulerLib VERSION 1.0 LANGUAGES CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the export definition
add_compile_definitions(TASKSCHEDULERLIB_EXPORTS)

# Add source files
set (sources
	"TaskScheduler.cxx"
)

#Add headers
set (includes
	"TaskScheduler.h"
)

# Add source to this project's executable.
add_library (${PROJECT_NAME} SHARED ${sources} ${includes})

# Specify include directories
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(TaskSchedulerLib PROPERTIES
    OUTPUT_NAME ${TASK_SCHEDULER_LIB_NAME}
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

install(TARGETS ${PROJECT_NAME}
     LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}
     ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}
     RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
 )

 if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(LibExtention ".lib")
 else()
    set(LibExtention ".so")
endif()

message(STATUS "INSTALL ${TASK_SCHEDULER_LIB_TARGET} DESTINATION: ${CMAKE_INSTALL_PREFIX}")

