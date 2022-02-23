cmake_minimum_required(VERSION 3.14)

project(FractalExplore)

add_subdirectory(fractal/dependencies/glfw/)

add_executable(${PROJECT_NAME} fractal/src/main.cpp fractal/src/math.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)