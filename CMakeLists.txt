cmake_minimum_required(VERSION 3.30)
project(FallingLeavesSimulation)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

set(IMGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg_installed/x64-linux/include)
set(IMGUI_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg_installed/x64-linux/lib/libimgui.a) 


file(GLOB SOURCES src/*.cpp)

include_directories(./include ${IMGUI_INCLUDE_DIR})

add_executable(falling_leaves ${SOURCES})

# target_include_directories(falling_leaves PRIVATE ${IMGUI_INCLUDE_DIR})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL3 REQUIRED)
#find_package(imgui REQUIRED) //Does not work for imgui for some reason

target_link_libraries(falling_leaves 
PRIVATE 
    SDL3::SDL3
    OpenGL::GL
    GLEW::GLEW
    ${IMGUI_LIBRARY}
)

# target_link_options(falling_leaves PRIVATE ${IMGUI_LIBRARY})