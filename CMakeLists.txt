cmake_minimum_required(VERSION 3.14)

project(FractalExplore)

add_subdirectory(dependencies/glfw/)
include_directories(include)

set(SOURCE_DIR src)
set(SOURCE
    ${SOURCE}
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/fractal.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE})
target_include_directories(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)