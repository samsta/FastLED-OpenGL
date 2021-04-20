# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (FastLED-OpenGL-Lib)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLM REQUIRED)

include_directories(
   .
   ${CMAKE_BINARY_DIR}
   )

set(ALL_LIBS
	${OPENGL_LIBRARY}
   ${GLEW_LIBRARIES}
   ${GLFW_LIBRARY}
)

add_definitions(
   -std=c++11
	-D_CRT_SECURE_NO_WARNINGS
   )

add_executable(toblob util/toblob.cpp)

add_executable(tutorial18_particles
	tutorial18_particles.cpp
	shader.cpp
	shader.hpp
	texture.cpp
	texture.hpp
	controls.cpp
	controls.hpp
	LED.fragmentshader
	LED.vertexshader
   "${CMAKE_BINARY_DIR}/LedTexture.cpp"
   )

add_custom_command(
   OUTPUT LedTexture.cpp
   COMMAND "${CMAKE_BINARY_DIR}/toblob"  "${CMAKE_CURRENT_SOURCE_DIR}/LED.DDS" "${CMAKE_BINARY_DIR}" LedTexture
   DEPENDS toblob LED.DDS
)

target_link_libraries(tutorial18_particles
	${ALL_LIBS}
)

# Xcode and Visual working directories
set_target_properties(tutorial18_particles PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tutorial18_billboards_and_particles/")

SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )