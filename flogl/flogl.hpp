#ifndef FLOGL_HPP
#define FLOGL_HPP

#include "platform.h"
#include "FastLED.h"
#include "Config.hpp"

namespace flogl {

struct LED{
   LED(float x, float y, float z, float size = 0.5):
         x(x), y(y), z(z),
         size(size),
         color(NULL)
   {
   }
	float x, y, z;
	float size;
   CRGB* color;
};

class Flogl
{
public:
   Flogl(std::vector<LED>& led_coordinates, const Config& config = Config());

   bool draw();
   
   ~Flogl();
private:
   class Impl;
   Impl& m_i;
};

}
#endif // FLOGL_HPP
