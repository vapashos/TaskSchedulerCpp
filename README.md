# TaskScheduler

TaskScheduler is a C++ project that implements a task scheduling system. It allows you to schedule tasks with different priorities and deadlines, and execute them using a thread pool.

## Features

- Schedule tasks with different priorities and deadlines
- Execute tasks using a thread pool
- Thread-safe task queue
- Condition variable for task synchronization

## Requirements

- CMake 3.21 or higher
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
    ./TaskScheduleDriver  [-h] [-nthreads <num_threads>] [-ntasks <num_tasks>]
    ```

## Project Structure

- `src/app`: Contains the main application code
- `src/lib`: Contains the task scheduler library code
- `build`: Directory for build files
- `install`: Directory for installed files





