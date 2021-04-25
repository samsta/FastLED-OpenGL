#ifndef FLOGL_HPP
#define FLOGL_HPP

#include "platform.h"
#include "FastLED.h"

namespace flogl {

struct LED{
   LED(double x, double y, double z):
         x(x), y(y), z(z),
         size(0.5),
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
