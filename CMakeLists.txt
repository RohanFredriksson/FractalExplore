cmake_minimum_required(VERSION 3.14)

project(FractalExplore)

add_subdirectory(fractal/dependencies/glfw/)

set(SOURCE_DIR fractal/src)
set(SOURCE
    ${SOURCE}
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/math.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE})
target_include_directories(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)