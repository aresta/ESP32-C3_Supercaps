#ifndef data_
#define data_
#include <stdint.h>

struct Sample {
  int16_t  temp;       // range: -50.0 - +50.0 celsius
  uint8_t  hum;        // range: 0% - 100%
  uint8_t  pres;       // range: 950 - 1050 millibars
  uint16_t volts;      // range: 0V - 5V
  uint8_t  error = 0;  // error code. No error = 0
  uint8_t  sleep_mins; // sleep interval when this sample was taken
  uint16_t seq_id;     // monotonic unique ID, stable across retries (random-seeded per cold boot)
};

const uint16_t max_samples = 7500 / sizeof(Sample); // RTC memory size is ~8K

#endif