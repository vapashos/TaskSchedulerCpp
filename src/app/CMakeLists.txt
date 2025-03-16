# CMakeList.txt : CMake project for TaskScheduleDriver, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.30)

project (TaskScheduleDriver VERSION 1.0 LANGUAGES CXX)	

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(sources 
	"main.cxx"
)

set(CMAKE_BUILD_TYPE Debug)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} ${sources} ${includes})

# Link against the shared library
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/${SRC_FOLDER}/${LIB_FOLDER}/${CMAKE_BUILD_TYPE}/${TASK_SCHEDULER_LIB})

# Include the library headers
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/lib)

# Install the executable
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION  ${CMAKE_INSTALL_PREFIX}
)


message(STATUS "APP:LIB BIN PATH: ${CMAKE_BINARY_DIR}/${SRC_FOLDER}/${LIB_FOLDER}/${CMAKE_BUILD_TYPE}/${TASK_SCHEDULER_LIB}")