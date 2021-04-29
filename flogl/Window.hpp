#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <glm/glm.hpp>

class GLFWwindow;

namespace flogl {

class Window
{
public:
   Window();
   ~Window();

   void processInputs();
   
   const glm::mat4& getViewMatrix() const;
   const glm::mat4& getProjectionMatrix() const;

   void swapBuffers();
   
   bool shouldClose() const;
   
private:
   static void doKeyCallback(GLFWwindow* window, int key, int scan_code, int action, int mods);
   void keyCallback(int key, int scan_code, int action, int mods);
   
   GLFWwindow*  m_window;
   glm::vec3    m_position;
   float        m_horizontal_angle_rad;
   float        m_vertical_angle_rad;
   float        m_fov_deg;
   float        m_speed;
   float        m_mouse_speed;

   glm::mat4    m_view_matrix;
   glm::mat4    m_projection_matrix;
   double       m_last_time;

   bool         m_control_mouse;
};

}
#endif
