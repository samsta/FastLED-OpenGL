#ifndef FLOGL_PLATFORM_H
#define FLOGL_PLATFORM_H

#include <stdint.h>

/*
  get FastLED's led_sysdefs.h and platforms.h out of the way as they will
  fail because they don't recognise our platform
*/
#define __INC_LED_SYSDEFS_H
#define __INC_PLATFORMS_H

#ifndef FASTLED_NAMESPACE_BEGIN
#define FASTLED_NAMESPACE_BEGIN
#define FASTLED_NAMESPACE_END
#define FASTLED_USING_NAMESPACE
#endif

#define FASTLED_FORCE_SOFTWARE_PINS
#define HAS_HARDWARE_PIN_SUPPORT

// likely to be problematic:
#define F_CPU 1000000000

// ARDUINO definitions
typedef uint32_t RwReg;
typedef uint32_t RoReg;

unsigned long micros();

#define pinMode(n, m)
#define digitalPinToBitMask(n) 0
#define digitalPinToPort(n) 0
#define portOutputRegister(n) NULL
#define portInputRegister(n) NULL



#endif // FLOGL_PLATFORM_H
