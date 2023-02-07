# Overview
This project provides the 2.5D robot simulator "Stage" as CMake compatible library to be used, e.g., with ROS.

See https://github.com/CodeFinder2/Stage for more information.

# Compiling
Getting started on Ubuntu 20.04:
```bash
sudo apt install git cmake build-essential libfltk1.3-dev libltdl-dev libpng-dev libjpeg-dev
git clone https://github.com/CodeFinder2/stage-simulator.git
cd stage-simulator && mkdir build && cd build
cmake .. && make -j$(nproc)
```

# How to use
```
include(cmake/CPM.cmake)
CPMAddPackage("gh:CodeFinder2/stage-simulator#master")
# ...
add_executable(my_app stage-simulator)
```

# Assets
All images, must be square in some 2^x number, to a maximum of 512x512 (2^9). The images should have depth 3 (RGB) or 4 (RGBA). They are compiled into the library as binary ressources.
