# FractalExplore C++

## Description
This project is the successor to my other project [FractalGenerator](https://github.com/RohanFredriksson/FractalGenerator). A simple window application that allows a user to view the marvels of fractal images in real time using shaders. The user can freely roam the intricate details of fractals by zooming and scrolling.

## Getting Started

To build and run this project, you'll need to have a C++ compiler and CMake installed on your machine. Follow these steps to set up the project:

### Prerequisites

Before you begin, ensure you have the following tools installed:

- **C++ Compiler**: You'll need a C++ compiler to build the project. If you don't have one already, you can install GCC or Clang.

- **CMake**: CMake is a build system generator that's used to configure and build the project. You can download and install it from [CMake's official website](https://cmake.org/download/).

### Clone the Repository

You can clone this Git repository to your local machine using the following command:

```shell
git clone https://github.com/RohanFredriksson/FractalExplore.git
```

### Install Submodules

This project has dependencies included as Git submodules. To ensure you have all the necessary dependencies, run the following commands in the project's root directory:

```shell
cd FractalExplore
git submodule update --init --recursive
```

This will clone and update all the submodules specified in the project.

### Build the Project
Next, enter the build directory and configure the project using CMake. Follow these steps:

```shell
cd build
cmake ..
```

### Compile the Program

Once CMake configuration is complete, you can build the program using make (or your platform-specific build command). Run the following command inside the build directory:

```
make
```

This will compile the project, and you should find the executable in the build directory or a specified output directory, depending on your CMake configuration.

You are now ready to run the program!

```shell
./FractalExplore
```

## Screenshots
![image info](preview.png)

## Reasoning
I started this project to learn how to build window based applications in lower level languages such as C and C++. I have always been interested in computer graphics and the technology related to it. A problem faced by the predecessor of this project **FractalGenerator** was the time that was required to compute just a single frame of a fractal. It would take several minutes to produce a still image even with parallelisation of the task. To improve the time taken to compute fractal images, I had to learn how to use the power of programming on the graphics card. Using GLSL shaders in OpenGL I could compute fractal images in real-time. 

## Contributions
Contributions are welcome! If you find any bugs or have suggestions for improvements, feel free to open an issue or submit a pull request on GitHub.
