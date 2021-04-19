# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (Tutorials)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLM REQUIRED)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)

include(CreateLaunchers)
#include(MSVCMultipleProcessCompile) # /MP

message(STATUS "mod path: ${CMAKE_MODULE_PATH}")

include_directories(.)

set(ALL_LIBS
	${OPENGL_LIBRARY}
   ${GLEW_LIBRARIES}
   ${GLFW_LIBRARY}
)

add_definitions(
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

# Xcode and Visual working directories
set_target_properties(tutorial18_particles PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tutorial18_billboards_and_particles/")
create_target_launcher(tutorial18_particles WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tutorial18_billboards_and_particles/")











SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )


if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

add_custom_command(
   TARGET tutorial18_particles POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/tutorial18_particles${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/tutorial18_billboards_and_particles/"
   )

elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

