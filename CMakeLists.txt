cmake_minimum_required (VERSION 3.0)
project (FastLED-OpenGL)

add_subdirectory(flogl)

include_directories(
   .
   )
add_executable(triangle examples/triangle.cpp)

target_link_libraries(triangle flogl)