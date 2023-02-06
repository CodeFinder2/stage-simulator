# Overview
This project provides the 2.5D robot simulator "Stage" as CMake compatible library to be used, e.g., with ROS.

See https://github.com/CodeFinder2/Stage for more information.

# How to use
```
include(cmake/CPM.cmake)
CPMAddPackage("gh:CodeFinder2/stage-simulator#main")
# ...
add_executable(my_app stage-simulator)
```

# Assets
All images, must be square in some 2^x number, to a maximum of 512x512 (2^9). The images should have depth 3 (RGB) or 4 (RGBA). They are compiled into the library as binary ressources.
