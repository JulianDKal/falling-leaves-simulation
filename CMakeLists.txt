cmake_minimum_required(VERSION 3.30)
project(FallingLeavesSimulation)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(./include)
file(GLOB SOURCES src/*.cpp)

add_executable(falling_leaves ${SOURCES})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL3 REQUIRED)

target_link_libraries(falling_leaves 
PRIVATE 
    SDL3::SDL3
    ${OPENGL_INCLUDE_DIR}
    GLEW::GLEW
)