#include "Window.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace flogl {

Window::Window():
      m_window(NULL),
      m_position(0, 8, 5),
      m_horizontal_angle_rad(3.14f),
      m_vertical_angle_rad(0.0f),
      m_fov_deg(45.0f),
      m_speed(20.0f),
      m_mouse_speed(0.001f),
      m_view_matrix(),
      m_projection_matrix(),
      m_last_time(0.0)
{
   // Initialise GLFW
   if(not glfwInit())
   {
      fprintf( stderr, "Failed to initialize GLFW\n" );
      exit(EXIT_FAILURE);
   }
     
   glfwWindowHint(GLFW_SAMPLES, 4);
   glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   // Open a window and create its OpenGL context
   int width = 1024;
   int height = 768;
   m_window = glfwCreateWindow(width, height, "FastLED OpenGL", NULL, NULL);
   if(m_window == NULL ){
      fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n" );
      glfwTerminate();
      exit(EXIT_FAILURE);
   }
   glfwMakeContextCurrent(m_window);
      
   // Initialize GLEW
   glewExperimental = true; // Needed for core profile
   if (glewInit() != GLEW_OK) {
      fprintf(stderr, "Failed to initialize GLEW\n");
      glfwTerminate();
      exit(EXIT_FAILURE);
   }
      
   // Ensure we can capture the escape key being pressed below
   glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);
   // Hide the mouse and enable unlimited mouvement
   glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      
   // Set the mouse at the center of the screen
   glfwPollEvents();
   glfwSetCursorPos(m_window, width/2, height/2);

   // black background
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

Window::~Window()
{      
   // Close OpenGL window and terminate GLFW
   glfwTerminate();   
}

void Window::processInputs()
{
   if (m_last_time == 0.0)
   {
      m_last_time = glfwGetTime();
   }

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - m_last_time);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(m_window, &xpos, &ypos);
	int width, height;
	glfwGetWindowSize(m_window, &width, &height);

	// Reset mouse position for next frame
	glfwSetCursorPos(m_window, width/2, height/2);

	// Compute new orientation
	m_horizontal_angle_rad += m_mouse_speed * float(width/2 - xpos );
	m_vertical_angle_rad   += m_mouse_speed * float(height/2 - ypos );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(m_vertical_angle_rad) * sin(m_horizontal_angle_rad), 
		sin(m_vertical_angle_rad),
		cos(m_vertical_angle_rad) * cos(m_horizontal_angle_rad)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(m_horizontal_angle_rad - 3.14f/2.0f), 
		0,
		cos(m_horizontal_angle_rad - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS){
		m_position += direction * deltaTime * m_speed;
	}
	// Move backward
	if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS){
		m_position -= direction * deltaTime * m_speed;
	}
	// Strafe right
	if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS){
		m_position += right * deltaTime * m_speed;
	}
	// Strafe left
	if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS){
		m_position -= right * deltaTime * m_speed;
	}

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	m_projection_matrix = glm::perspective(glm::radians(m_fov_deg), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	m_view_matrix = glm::lookAt(
								m_position,           // Camera is here
								m_position+direction, // and looks here : at the same position, plus "direction"
								up                    // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	m_last_time = currentTime;
}


const glm::mat4& Window::getViewMatrix() const
{
   return m_view_matrix;
}

const glm::mat4& Window::getProjectionMatrix() const
{
   return m_projection_matrix;
}

void Window::swapBuffers()
{
   glfwSwapBuffers(m_window);
   glfwPollEvents();
}

bool Window::shouldClose() const
{
   return glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS or
          glfwWindowShouldClose(m_window);
}


}
