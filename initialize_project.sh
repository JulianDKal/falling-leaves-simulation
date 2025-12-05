#!/bin/bash

#initialize the submodule (vcpkg) from the information specified in .gitmodules
git submodule init
git submodule update

#initialize vcpkg
cd vcpkg
./bootstrap-vcpkg.sh
vcpkg new --application
#specify the dependencies to download for vcpkg
echo '{
  "name": "falling-leaves-simulation",
  "version": "1.0.0",
  "description": "An app that simulates large numbers of falling leaves",
  "dependencies": [
    "sdl3",
    "opengl",
    "glew",
    {
      "name": "imgui",
      "features": ["sdl3-binding", "opengl3-binding"]
    },
    "glm"
  ]
}' > vcpkg.json

vcpkg install

cd ..

#instruct cmake to use the toolchain file from vcpkg
cmake -B build -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build