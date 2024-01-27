#ifndef data_
#define data_
#include <stdint.h>

struct Sample {
  int16_t temp; // range: -50.0 - +50.0 celsius
  uint8_t hum;  // range: 0% - 100%
  uint8_t pres; // range: 950 - 1050 millibars
  uint8_t volts;// range: 0V - 5V
  uint16_t sleep_mins;
  };

const uint8_t sample_size = 2+1+1+1+2;
const uint8_t max_samples = uint8_t( 7500 / sample_size); // RTC memory size is 8K

#endif