cmake_minimum_required(VERSION 3.30)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
project(FallingLeavesSimulation)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Option to choose target platform
option(CROSS_COMPILE_WINDOWS "Cross-compile to Windows" OFF)

if(CROSS_COMPILE_WINDOWS)
    message(STATUS "Cross-compiling to Windows")
    # set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg_installed/x64-mingw-static")
    set(VCPKG_TARGET_TRIPLET "x64-mingw-static")
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
    set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
    
    # Static linking for Windows
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
    
    # Windows paths
    set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg_installed/x64-mingw-static/include)
    set(IMGUI_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg_installed/x64-mingw-static/lib/libimgui.a)
    set(SDL3_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg_installed/x64-mingw-static/lib/libSDL3.a)
    set(GLEW_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg_installed/x64-mingw-static/lib/libglew32.a)
    set(GLM_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg_installed/x64-mingw-static/lib/libglm.a)

    set (MINGW_SYSROOT /usr/x86_64-w64-mingw32/sys-root/mingw)


else()
    message(STATUS "Building for native Linux")
    set(VCPKG_TARGET_TRIPLET "x64-linux")
    
    # Linux paths
    set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg_installed/x64-linux/include)
    set(IMGUI_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg_installed/x64-linux/lib/libimgui.a)
    set(SDL3_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg_installed/x64-linux/lib/libSDL3.a)
    set(GLEW_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg_installed/x64-linux/lib/libGLEW.a)
    set(GLM_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg_installed/x64-linux/lib/libglm.a)
endif()


file(GLOB SOURCES src/*.cpp)

include_directories(./include ${INCLUDE_DIR})

add_executable(falling_leaves ${SOURCES})

if(CROSS_COMPILE_WINDOWS)
    #link all relevant libraries for windows cross compiling
    target_link_libraries(falling_leaves 
    PRIVATE 
    ${SDL3_LIBRARY}
    ${GLEW_LIBRARY}
    ${IMGUI_LIBRARY}
    ${GLM_LIBRARY}
    ${MINGW_SYSROOT}/lib/libopengl32.a
    ${MINGW_SYSROOT}/lib/libgdi32.a
    ${MINGW_SYSROOT}/lib/libuser32.a
    ${MINGW_SYSROOT}/lib/libwinmm.a
    ${MINGW_SYSROOT}/lib/libole32.a
    ${MINGW_SYSROOT}/lib/liboleaut32.a
    ${MINGW_SYSROOT}/lib/libsetupapi.a
    ${MINGW_SYSROOT}/lib/libimm32.a
    ${MINGW_SYSROOT}/lib/libversion.a
    ${MINGW_SYSROOT}/lib/libuuid.a
    ${MINGW_SYSROOT}/lib/libshell32.a
)
else()
    #link all relevant libraries for native linux
    find_package(OpenGL REQUIRED)
    target_link_libraries(falling_leaves 
    PRIVATE
    ${SDL3_LIBRARY}
    OpenGL::GL
    ${GLEW_LIBRARY}
    ${IMGUI_LIBRARY}
    ${GLM_LIBRARY}
    )
endif()

#find_package(GLEW REQUIRED)
#find_package(SDL3 REQUIRED)
#find_package(glm REQUIRED)
#find_package(imgui REQUIRED) //Does not work for imgui for some reason

