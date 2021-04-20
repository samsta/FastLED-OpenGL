# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (FastLED-OpenGL-Lib)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLM REQUIRED)

include_directories(.)

set(ALL_LIBS
	${OPENGL_LIBRARY}
   ${GLEW_LIBRARIES}
   ${GLFW_LIBRARY}
)

add_definitions(
   -std=c++11
	-D_CRT_SECURE_NO_WARNINGS
)

add_executable(tutorial18_particles
	tutorial18_billboards_and_particles/tutorial18_particles.cpp
	common/shader.cpp
	common/shader.hpp
	common/texture.cpp
	common/texture.hpp
	common/controls.cpp
	common/controls.hpp
	tutorial18_billboards_and_particles/Particle.fragmentshader
	tutorial18_billboards_and_particles/Particle.vertexshader
)

target_link_libraries(tutorial18_particles
	${ALL_LIBS}
)

add_custom_target(copy_dds ALL cp "${CMAKE_CURRENT_SOURCE_DIR}/tutorial18_billboards_and_particles/particle.DDS" "${CMAKE_CURRENT_BINARY_DIR}")

# Xcode and Visual working directories
set_target_properties(tutorial18_particles PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tutorial18_billboards_and_particles/")

SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )