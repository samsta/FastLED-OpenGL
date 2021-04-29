#include "flogl.hpp"
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <algorithm>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>


#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"

namespace flogl {

using namespace glm;

namespace {
const std::string VERTEX_SHADER =
#include "LED.vertexshader"
;

const std::string FRAGMENT_SHADER =
#include "LED.fragmentshader"
;
}

#include "LedTexture.cpp"

class Flogl::Impl
{
public:
   Impl(LED* leds, unsigned num_leds);  
   ~Impl();

   void add(CRGB* leds, unsigned num_leds);
   bool draw();
      
   LED*           m_leds;
   const unsigned m_num_leds;
   GLFWwindow*    m_window;
   GLuint         m_vertex_array_id;
   GLuint         m_program_id;
   GLfloat*       m_led_position_size_data;
   GLubyte*       m_led_color_data;
   GLuint         m_vertex_buffer;
   GLuint         m_leds_position_buffer;
   GLuint         m_leds_color_buffer;
   GLuint         m_texture;
   GLuint         m_camera_right_worldspace_id;
   GLuint         m_camera_up_worldspace_id;
   GLuint         m_view_proj_matrix_id;
   GLuint         m_texture_id;
};

Flogl::Impl::Impl(LED* leds, unsigned num_leds):
   m_leds(leds),
   m_num_leds(num_leds),
   m_window(NULL),
   m_led_position_size_data(new GLfloat[num_leds * 4]),
   m_led_color_data(new GLubyte[num_leds * 4])
{
   // Initialise GLFW
   if(not glfwInit())
   {
      fprintf( stderr, "Failed to initialize GLFW\n" );
      return;
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
      return;
   }
   glfwMakeContextCurrent(m_window);
      
   // Initialize GLEW
   glewExperimental = true; // Needed for core profile
   if (glewInit() != GLEW_OK) {
      fprintf(stderr, "Failed to initialize GLEW\n");
      glfwTerminate();
      return;
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
      
   // Enable depth test
   glEnable(GL_DEPTH_TEST);
   // Accept fragment if it closer to the camera than the former one
   glDepthFunc(GL_LESS);
      
   glGenVertexArrays(1, &m_vertex_array_id);
   glBindVertexArray(m_vertex_array_id);
      
   // Create and compile our GLSL program from the shaders
   m_program_id = LoadShaders(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
      
   // Vertex shader
   m_camera_right_worldspace_id  = glGetUniformLocation(m_program_id, "CameraRight_worldspace");
   m_camera_up_worldspace_id  = glGetUniformLocation(m_program_id, "CameraUp_worldspace");
   m_view_proj_matrix_id = glGetUniformLocation(m_program_id, "VP");
      
   // fragment shader
   m_texture_id  = glGetUniformLocation(m_program_id, "myTextureSampler");
            
   m_texture = loadDDS(blobs::LedTexture, sizeof(blobs::LedTexture));
      
   // The VBO containing the 4 vertices of the leds.
   // Thanks to instancing, they will be shared by all leds.
   static const GLfloat g_vertex_buffer_data[] = { 
     -0.5f, -0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
     -0.5f,  0.5f, 0.0f,
      0.5f,  0.5f, 0.0f,
   };

   glGenBuffers(1, &m_vertex_buffer);
   glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
      
   // The VBO containing the positions and sizes of the leds
   glGenBuffers(1, &m_leds_position_buffer);
   glBindBuffer(GL_ARRAY_BUFFER, m_leds_position_buffer);
   // Initialize with empty (NULL) buffer : it will be updated later, each frame.
   glBufferData(GL_ARRAY_BUFFER, m_num_leds * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
      
   // The VBO containing the colors of the leds
   glGenBuffers(1, &m_leds_color_buffer);
   glBindBuffer(GL_ARRAY_BUFFER, m_leds_color_buffer);
   // Initialize with empty (NULL) buffer : it will be updated later, each frame.
   glBufferData(GL_ARRAY_BUFFER, m_num_leds * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

}

void Flogl::Impl::add(CRGB* leds, unsigned num_leds)
{
   for (unsigned k = 0; k < num_leds && k < m_num_leds; k++)
   {
      m_leds[k].led = &leds[k];
   }
}


bool Flogl::Impl::draw()
{
   // Clear the screen
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   computeMatricesFromInputs(m_window);
   glm::mat4 ProjectionMatrix = getProjectionMatrix();
   glm::mat4 ViewMatrix = getViewMatrix();
   
   
   // We will need the camera's position in order to sort the leds
   // w.r.t the camera's distance.
   // There should be a getCameraPosition() function in common/controls.cpp, 
   // but this works too.
   glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);
   
   glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
   
   
   for(int i=0; i<m_num_leds; i++){
      
      LED& p = m_leds[i]; // shortcut
      
      // Fill the GPU buffer
      m_led_position_size_data[4*i+0] = p.x;
      m_led_position_size_data[4*i+1] = p.y;
      m_led_position_size_data[4*i+2] = p.z;
      
      m_led_position_size_data[4*i+3] = p.size;
      
      m_led_color_data[4*i+0] = p.led ? p.led->red : 0;
      m_led_color_data[4*i+1] = p.led ? p.led->green : 0;
      m_led_color_data[4*i+2] = p.led ? p.led->blue : 0;
      m_led_color_data[4*i+3] = 255;
      
   }
   
   // Update the buffers that OpenGL uses for rendering.
   // There are much more sophisticated means to stream data from the CPU to the GPU, 
   // but this is outside the scope of this tutorial.
   // http://www.opengl.org/wiki/Buffer_Object_Streaming
   
   
   glBindBuffer(GL_ARRAY_BUFFER, m_leds_position_buffer);
   glBufferData(GL_ARRAY_BUFFER, m_num_leds * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
   glBufferSubData(GL_ARRAY_BUFFER, 0, m_num_leds * sizeof(GLfloat) * 4, m_led_position_size_data);
   
   glBindBuffer(GL_ARRAY_BUFFER, m_leds_color_buffer);
   glBufferData(GL_ARRAY_BUFFER, m_num_leds * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
   glBufferSubData(GL_ARRAY_BUFFER, 0, m_num_leds * sizeof(GLubyte) * 4, m_led_color_data);
   
   
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
   // Use our shader
   glUseProgram(m_program_id);
   
   // Bind our texture in Texture Unit 0
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, m_texture);
   // Set our "myTextureSampler" sampler to use Texture Unit 0
   glUniform1i(m_texture_id, 0);
   
   // Same as the billboards tutorial
   glUniform3f(m_camera_right_worldspace_id, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
   glUniform3f(m_camera_up_worldspace_id   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
   
   glUniformMatrix4fv(m_view_proj_matrix_id, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);
   
   // 1rst attribute buffer : vertices
   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
   glVertexAttribPointer(
      0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
      3,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
   );
   
   // 2nd attribute buffer : positions of leds' centers
   glEnableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER, m_leds_position_buffer);
   glVertexAttribPointer(
      1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
      4,                                // size : x + y + z + size => 4
      GL_FLOAT,                         // type
      GL_FALSE,                         // normalized?
      0,                                // stride
      (void*)0                          // array buffer offset
   );
   
   // 3rd attribute buffer : leds' colors
   glEnableVertexAttribArray(2);
   glBindBuffer(GL_ARRAY_BUFFER, m_leds_color_buffer);
   glVertexAttribPointer(
      2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
      4,                                // size : r + g + b + a => 4
      GL_UNSIGNED_BYTE,                 // type
      GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
      0,                                // stride
      (void*)0                          // array buffer offset
   );
   
   // These functions are specific to glDrawArrays*Instanced*.
   // The first parameter is the attribute buffer we're talking about.
   // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
   // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
   glVertexAttribDivisor(0, 0); // leds vertices : always reuse the same 4 vertices -> 0
   glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
   glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1
   
   // Draw the leds !
   // This draws many times a small triangle_strip (which looks like a quad).
   // This is equivalent to :
   // for(i in LEDsCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
   // but faster.
   glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_num_leds);
   
   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(2);
   
   // Swap buffers
   glfwSwapBuffers(m_window);
   glfwPollEvents();
   
   
   return glfwGetKey(m_window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(m_window) == 0;
}

Flogl::Impl::~Impl()
{
   delete[] m_led_position_size_data;
   delete[] m_led_color_data;
   
   // Cleanup VBO and shader
   glDeleteBuffers(1, &m_leds_color_buffer);
   glDeleteBuffers(1, &m_leds_position_buffer);
   glDeleteBuffers(1, &m_vertex_buffer);
   glDeleteProgram(m_program_id);
   glDeleteTextures(1, &m_texture);
   glDeleteVertexArrays(1, &m_vertex_array_id);
   
   // Close OpenGL window and terminate GLFW
   glfwTerminate();   
}

Flogl::Flogl(LED* leds, unsigned num_leds):
   m_i(*new Flogl::Impl(leds, num_leds))
{
}

void Flogl::add(CRGB* leds, unsigned num_leds)
{
   m_i.add(leds, num_leds);
}

bool Flogl::draw()
{
   return m_i.draw();
}

Flogl::~Flogl()
{
   delete &m_i;
}

}
