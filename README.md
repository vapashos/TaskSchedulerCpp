# TaskScheduler

TaskScheduler is a C++ project that implements a task scheduling system. It allows you to schedule tasks with different priorities and deadlines, and execute them using a thread pool.

## Features

- Schedule tasks with different priorities and deadlines
- Execute tasks using a thread pool
- Thread-safe task queue
- Condition variable for task synchronization

## Requirements

- CMake 3.30 or higher
- C++17 compatible compiler

## Building the Project

1. Clone the repository:
    ```sh
    git clone https://github.com/yourusername/TaskScheduler.git
    cd TaskScheduler
    ```

2. Create a build directory and navigate to it:
    ```sh
    mkdir build
    cd build
    ```

3. Run CMake to configure the project:
    ```sh
    cmake ..
    ```

4. Build the project:
    ```sh
    cmake --build .
    ```

## Running the Project

1. After building the project, navigate to the `install` directory:
    ```sh
    cd ../install
    ```

2. Run the executable:
    ```sh
    ./TaskScheduleDriver
    ```

## Project Structure

- `src/app`: Contains the main application code
- `src/lib`: Contains the task scheduler library code
- `build`: Directory for build files
- `install`: Directory for installed files

## Usage

To use the TaskScheduler library in your project, include the header files and link against the library:

```cpp
#include <TaskScheduler.h>

// Create a task scheduler with 4 threads
std::condition_variable cv;
ITaskSchedulerPtr scheduler = CreateScheduler(4, cv);

// Schedule tasks
scheduler->Schedule([]() { std::cout << "Task 1\n"; }, 10, 0.0);
scheduler->Schedule([]() { std::cout << "Task 2\n"; }, 90, 0.0);

// Run the scheduler
scheduler->Run();

// Terminate the scheduler
scheduler->Terminate();
