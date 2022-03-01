cmake_minimum_required(VERSION 3.14)

project(FractalExplore)

add_subdirectory(dependencies/glfw/)
add_subdirectory(dependencies/glad/)
add_subdirectory(dependencies/glm/)

include_directories(include)

set(SOURCE_DIR src)
set(SOURCE
    ${SOURCE}
    src/main.cpp
    src/window.cpp
    src/shader.cpp
    src/fractal.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE})

target_include_directories(${PROJECT_NAME} PUBLIC glad)
target_include_directories(${PROJECT_NAME} PUBLIC glfw)
target_include_directories(${PROJECT_NAME} PUBLIC glm)

target_link_libraries(${PROJECT_NAME} PUBLIC glad)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC glm)