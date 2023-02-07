cmake_minimum_required(VERSION 3.11)
project(stage-simulator VERSION 1.0.0)

set(default_build_type "Release")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Dependencies:
set(FLTK_SKIP_FLUID TRUE)
find_package(FLTK REQUIRED)
set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
if(NOT OPENGL_GLU_FOUND)
  message(FATAL_ERROR "OpenGL GLU not found, aborting.")
endif()
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
include(cmake/CPM.cmake)
CPMAddPackage("gh:CodeFinder2/embed-resource@0.1")
message(STATUS "Checking for Libtool.")
find_path(LTDL_INCLUDE_DIR ltdl.h DOC "Libtool include dir")
find_library(LTDL_LIB ltdl DOC "Libtool lib")

# Declare library and settings:
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cc)
embed_resources(ASSETS
  assets/rgb.txt
  assets/stagelogo.png
  assets/mainspower.png
  assets/stall.png
)
add_library(${PROJECT_NAME} ${SRC_FILES} ${ASSETS})
target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -pedantic -Wextra)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}
  ${LTDL_INCLUDE_DIR}
  ${PNG_INCLUDE_DIR}
  ${JPEG_INCLUDE_DIR}
)
target_link_libraries(${PROJECT_NAME}
  embed_resource
  ${FLTK_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${LTDL_LIB}
  ${PNG_LIBRARIES}
  ${JPEG_LIBRARIES}
)
configure_file(include/${PROJECT_NAME}/config.hh.in ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/config.hh @ONLY)

# Install section:
include(GNUInstallDirs)
file(GLOB_RECURSE HDR_FILES CONFIGURE_DEPENDS include/*.hh ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/config.hh)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HDR_FILES}")
install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)
