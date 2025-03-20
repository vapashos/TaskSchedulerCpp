# CMakeList.txt : CMake project for TaskSchedulerDriver, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.21)

project (TaskSchedulerDriver VERSION 1.0 LANGUAGES CXX)	

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(sources 
	"main.cxx"
)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} ${sources} ${includes})

# Include the library headers
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src/lib)

# Link against the shared library
target_link_libraries(${PROJECT_NAME} PUBLIC ${TASK_SCHEDULER_LIB_BINARIES}/${TASK_SCHEDULER_LIB_TARGET})


# Install the executable
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION  ${CMAKE_INSTALL_PREFIX}
)

message(STATUS "INSTALL ${PROJECT_NAME}.exe DESTINATION: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "APP TaskSchedulerDriver INCLUDE DIRECTORIES : ${CMAKE_SOURCE_DIR}/src/lib")
