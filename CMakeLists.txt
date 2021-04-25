cmake_minimum_required (VERSION 3.0)
project (FastLED-OpenGL)

set(FASTLED_DIR "" CACHE PATH "Path to FastLED installation")

if(FASTLED_DIR STREQUAL "")
   message("FASTLED_DIR not set, searching default locations")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/flogl/cmake/Modules")
find_package(FastLED REQUIRED)

add_subdirectory(flogl)
add_definitions(
   -std=c++11
   )

include_directories(
   flogl
   .
   )

add_executable(triangle examples/triangle.cpp)

target_link_libraries(triangle flogl)