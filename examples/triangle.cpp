#include "flogl/flogl.hpp"
#include <math.h>
#include <unistd.h>

float vsc = 0.5;
float hsc = 0.3;

using flogl::LED;

LED leds[] =
{
   LED(hsc*0,  vsc*30, -20),
   LED(hsc*1,  vsc*29, -20),
   LED(hsc*2,  vsc*28, -20),
   LED(hsc*3,  vsc*27, -20),
   LED(hsc*4,  vsc*26, -20),
   LED(hsc*5,  vsc*25, -20),
   LED(hsc*6,  vsc*24, -20),
   LED(hsc*7,  vsc*23, -20),
   LED(hsc*8,  vsc*22, -20),
   LED(hsc*9,  vsc*21, -20),
   LED(hsc*10, vsc*20, -20),
   LED(hsc*11, vsc*19, -20),
   LED(hsc*12, vsc*18, -20),
   LED(hsc*13, vsc*17, -20),
   LED(hsc*14, vsc*16, -20),
   LED(hsc*15, vsc*15, -20),
   LED(hsc*16, vsc*14, -20),
   LED(hsc*17, vsc*13, -20),
   LED(hsc*18, vsc*12, -20),
   LED(hsc*19, vsc*11, -20),
   LED(hsc*20, vsc*10, -20),
   LED(hsc*21, vsc*9, -20),
   LED(hsc*22, vsc*8, -20),
   LED(hsc*23, vsc*7, -20),
   LED(hsc*24, vsc*6, -20),
   LED(hsc*25, vsc*5, -20),
   LED(hsc*26, vsc*4, -20),
   LED(hsc*27, vsc*3, -20),
   LED(hsc*28, vsc*2, -20),
   LED(hsc*29, vsc*1, -20),
   LED(hsc*30, vsc*0, -20),

   LED(hsc*28,  0, -20),
   LED(hsc*26,  0, -20),
   LED(hsc*24,  0, -20),
   LED(hsc*22,  0, -20),
   LED(hsc*20,  0, -20),
   LED(hsc*18,  0, -20),
   LED(hsc*16,  0, -20),
   LED(hsc*14,  0, -20),
   LED(hsc*12,  0, -20),
   LED(hsc*10,  0, -20),
   LED(hsc*8,   0, -20),
   LED(hsc*6,   0, -20),
   LED(hsc*4,   0, -20),
   LED(hsc*2,   0, -20),
   LED(hsc*0,   0, -20),
   LED(-hsc*2,   0, -20),
   LED(-hsc*4,   0, -20),
   LED(-hsc*6,   0, -20),
   LED(-hsc*8,   0, -20),
   LED(-hsc*10,  0, -20),
   LED(-hsc*12,  0, -20),
   LED(-hsc*14,  0, -20),
   LED(-hsc*16,  0, -20),
   LED(-hsc*18,  0, -20),
   LED(-hsc*20,  0, -20),
   LED(-hsc*22,  0, -20),
   LED(-hsc*24,  0, -20),
   LED(-hsc*26,  0, -20),
   LED(-hsc*28,  0, -20),

   
   LED(-hsc*30, vsc*0, -20),
   LED(-hsc*29, vsc*1, -20),
   LED(-hsc*28, vsc*2, -20),
   LED(-hsc*27, vsc*3, -20),
   LED(-hsc*26, vsc*4, -20),
   LED(-hsc*25, vsc*5, -20),
   LED(-hsc*24, vsc*6, -20),
   LED(-hsc*23, vsc*7, -20),
   LED(-hsc*22, vsc*8, -20),
   LED(-hsc*21, vsc*9, -20),
   LED(-hsc*20, vsc*10, -20),
   LED(-hsc*19, vsc*11, -20),
   LED(-hsc*18, vsc*12, -20),
   LED(-hsc*17, vsc*13, -20),
   LED(-hsc*16, vsc*14, -20),
   LED(-hsc*15, vsc*15, -20),
   LED(-hsc*14, vsc*16, -20),
   LED(-hsc*13, vsc*17, -20),
   LED(-hsc*12, vsc*18, -20),
   LED(-hsc*11, vsc*19, -20),
   LED(-hsc*10, vsc*20, -20),
   LED(-hsc*9,  vsc*21, -20),
   LED(-hsc*8,  vsc*22, -20),
   LED(-hsc*7,  vsc*23, -20),
   LED(-hsc*6,  vsc*24, -20),
   LED(-hsc*5,  vsc*25, -20),
   LED(-hsc*4,  vsc*26, -20),
   LED(-hsc*3,  vsc*27, -20),
   LED(-hsc*2,  vsc*28, -20),
   LED(-hsc*1,  vsc*29, -20),
   LED(-hsc*0,  vsc*30, -20),
};

void loop();

int main()
{
   flogl::Flogl flogl(leds, sizeof(leds)/sizeof(*leds));

   do {
      usleep(30000);
      loop();
   } while(flogl.draw());
   
   return 0;
}

void loop()
{
   static unsigned cnt = 0;
   const unsigned num_leds = sizeof(leds)/sizeof(*leds);
   
   for(int i=0; i < num_leds; i++)
   {
      float t = float(cnt)/10;
      float a = 2 * 3.141 * float(i)/num_leds;
      
      leds[i].r = (sin(t  + a) + 1) * 127;
      leds[i].g = (cos(t  + a) + 1) * 127;
      leds[i].b = (sin(-t - a) + 1) * 127;
   }

   cnt++;
}
