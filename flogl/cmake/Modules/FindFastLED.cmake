# FindFastLED - attempts to locate the FastLED sources

find_path(FastLED_BASE "src/FastLED.h"
          PATHS "${FASTLED_DIR}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FastLED DEFAULT_MSG FastLED_BASE)

set(FastLED_SRC     "${FASTLED_DIR}/src")
set(FastLED_INCLUDE "${FASTLED_DIR}/src")

include_directories(SYSTEM "${FastLED_INCLUDE}")
