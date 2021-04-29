#ifndef FLOGL_HPP
#define FLOGL_HPP

#include "platform.h"
#include "FastLED.h"

namespace flogl {

struct LED{
   LED(float x, float y, float z, float size = 0.5):
         x(x), y(y), z(z),
         size(size),
         led(NULL)
   {
   }
	float x, y, z;
	float size;
   CRGB* led;
};

class Flogl
{
public:
   Flogl(LED* led_coordinates, unsigned num_leds);

   void add(CRGB* leds, unsigned num_leds);

   bool draw();
   
   ~Flogl();
private:
   class Impl;
   Impl& m_i;
};

}
#endif // FLOGL_HPP
