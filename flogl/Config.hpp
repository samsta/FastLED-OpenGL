#ifndef FLOGL_CONFIG_HPP
#define FLOGL_CONFIG_HPP

#include <vector>

namespace flogl {
  
class Config
{
public:
   
   struct View
   {
      View(int x, int y, int z, float fov_deg = 45.f, float horizontal_angle_deg = 0.f, float vertical_angle_deg = 0.f):
         x(x),
         y(y),
         z(z),
         fov_deg(fov_deg),
         horizontal_angle_deg(horizontal_angle_deg),
         vertical_angle_deg(vertical_angle_deg)
      {}
      int x, y, z;
      float fov_deg, horizontal_angle_deg, vertical_angle_deg;
   };
   
   
   
   Config():
      m_width(1024),
      m_height(768),
      m_views(1, View(0, 8, 5))
   {
   }
   
   Config& width(int width) { m_width = width; return *this; }
   int width() const { return m_width;}
   
   Config& height(int height) { m_height = height; return *this; }
   int height() const { return m_height; }
   
   Config& views(std::vector<View>& views) { m_views = views; return *this; }
   const std::vector<View>& views() const { return m_views; }
   
private:
   int m_width;
   int m_height;
   std::vector<View> m_views;
};

}

#endif // FLOGL_CONFIG_HPP
