# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (FastLED-OpenGL-Lib)

find_package(OpenGL REQUIRED)
if (NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
   find_package(GLEW REQUIRED)
   add_definitions(-DWITH_GLEW=1)
endif()
find_package(GLFW REQUIRED)
find_package(GLM REQUIRED)

include_directories(
   .
   ${CMAKE_CURRENT_BINARY_DIR}
   ${GLFW_INCLUDE_DIR}
   ${GLM_INCLUDE_DIR}
   )

set(ALL_LIBS
   ${OPENGL_LIBRARY}
   ${GLEW_LIBRARIES}
   ${GLFW_LIBRARY}
)


add_executable(toblob util/toblob.cpp)

set(FASTLED_SOURCES
   "${FastLED_SRC}/colorutils.cpp"
   "${FastLED_SRC}/colorpalettes.cpp"
   "${FastLED_SRC}/hsv2rgb.cpp"
   "${FastLED_SRC}/lib8tion.cpp"
   )
set_source_files_properties(${FASTLED_SOURCES}
   PROPERTIES COMPILE_FLAGS "-include platform.h -Wno-mismatched-tags -Wno-unused-private-field"
   )
source_group(FastLED FILES ${FASTLED_SOURCES})

set(FLOGL_SOURCES
   flogl.cpp
   shader.cpp
   texture.cpp
   Window.cpp
   "${CMAKE_CURRENT_BINARY_DIR}/LedTexture.hpp"
   )

# Disable annoying #pragma messages.
set_source_files_properties(${FLOGL_SOURCES} PROPERTIES COMPILE_FLAGS " -DFASTLED_INTERNAL")

add_library(flogl
   ${FLOGL_SOURCES}
   LED.fragmentshader
   LED.vertexshader
   ${FASTLED_SOURCES}
   )

add_custom_command(
   OUTPUT LedTexture.hpp
   COMMAND toblob  "${CMAKE_CURRENT_SOURCE_DIR}/LED.DDS" "${CMAKE_CURRENT_BINARY_DIR}" LedTexture
   DEPENDS toblob LED.DDS
)

target_link_libraries(flogl
   ${ALL_LIBS}
   )
