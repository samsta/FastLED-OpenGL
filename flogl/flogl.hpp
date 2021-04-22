#ifndef FLOGL_HPP
#define FLOGL_HPP

struct GLFWwindow;

namespace flogl {

struct LED{
   LED(double x, double y, double z):
         x(x), y(y), z(z),
         size(0.5)
   {
   }
	float x, y, z;
	unsigned char r, g, b;
	float size;
};

class Flogl
{
public:
   Flogl(LED* leds, unsigned num_leds);
   
private:
   LED*           m_leds;
   const unsigned m_num_leds;
   
   GLFWwindow*    m_window;
};

}
#endif // FLOGL_HPP
