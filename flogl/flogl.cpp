#include "flogl.hpp"
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <algorithm>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#ifdef WITH_GLEW
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "texture.hpp"
#include "Window.hpp"

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
   Impl(LED* leds, unsigned num_leds, const Config& config);  
   ~Impl();

   void add(CRGB* leds, unsigned num_leds);
   bool draw();
      
   LED*           m_leds;
   const unsigned m_num_leds;
   Window         m_window;
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

Flogl::Impl::Impl(LED* leds, unsigned num_leds, const Config& config):
   m_leds(leds),
   m_num_leds(num_leds),
   m_window(config),
   m_led_position_size_data(new GLfloat[num_leds * 4]),
   m_led_color_data(new GLubyte[num_leds * 4])
{
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
   m_window.processInputs();

   const glm::mat4& view_matrix = m_window.getViewMatrix();
   glm::vec3 camera_position(glm::inverse(view_matrix)[3]);
   glm::mat4 ViewProjectionMatrix = m_window.getProjectionMatrix() * view_matrix;
   
   float closest_led_distance = 100000.f;
   for(int i=0; i<m_num_leds; i++){
       LED& p = m_leds[i];
       closest_led_distance = std::min(closest_led_distance, glm::length2(glm::vec3(p.x, p.y, p.z) - camera_position));
   }

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
    
      // dim LEDs further in the background, but don't allow them to disappear completely
      float scaled_distance = (glm::length2(glm::vec3(p.x, p.y, p.z) - camera_position) - closest_led_distance)/300;
      m_led_color_data[4*i+3] = 255 - std::min(scaled_distance, 200.f);
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
   glUniform3f(m_camera_right_worldspace_id, view_matrix[0][0], view_matrix[1][0], view_matrix[2][0]);
   glUniform3f(m_camera_up_worldspace_id   , view_matrix[0][1], view_matrix[1][1], view_matrix[2][1]);
   
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
   // for(i in m_num_leds) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
   // but faster.
   glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_num_leds);
   
   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(2);
   
   m_window.swapBuffers();
   
   return not m_window.shouldClose();
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
}

Flogl::Flogl(LED* leds, unsigned num_leds, const Config& config):
    m_i(*new Flogl::Impl(leds, num_leds, config))
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
