cmake_minimum_required(VERSION 3.11)
project(stage-simulator VERSION 4.4.0)

# Dependencies:
find_package(FLTK REQUIRED)
find_package(OpenGL REQUIRED)
if(NOT OPENGL_GLU_FOUND)
  message(FATAL_ERROR "OpenGL GLU not found, aborting")
endif()
include(cmake/CPM.cmake)
CPMAddPackage("gh:CodeFinder2/embed-resource#master")

# Declare library and settings:
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cc)
embed_resources(ASSETS
  assets/rgb.txt
  assets/stagelogo.png
  assets/stall.png
)
add_library(${PROJECT_NAME} ${SRC_FILES} ${ASSETS})
target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall) # TODO: allow -Wextra -pedantic
target_link_libraries(${PROJECT_NAME} embed_resource)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
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