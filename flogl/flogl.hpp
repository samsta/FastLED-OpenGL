#ifndef FLOGL_HPP
#define FLOGL_HPP

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

   bool draw();
   
   ~Flogl();
private:
   class Impl;
   Impl& m_i;
};

}
#endif // FLOGL_HPP
