#include <Arduino.h>
#include <Adafruit_BME280.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "WiFi.h"
#include <Wire.h>
#include <esp_sleep.h>
#include "data.h"
#include "conn.h"
#include "mqtt.h"
#include "conf.h"

#define LED GPIO_NUM_8
#define SDA_PIN 6
#define SCL_PIN 5
#define ADC_VOLT_PIN 3

Adafruit_BME280 bme;
WiFiClientSecure wifiClient;
PubSubClient awsClient;

uint32_t init_time = 0;

RTC_DATA_ATTR Sample    samples[ max_samples];
RTC_DATA_ATTR uint16_t  sample_count_total = 0;
RTC_DATA_ATTR uint16_t  sample_count_new = 0;
RTC_DATA_ATTR uint8_t   sleep_mins = SHORT_SLEEP_MIN;
RTC_DATA_ATTR uint8_t   samples_to_send = PACK_SIZE_NORMAL;
RTC_DATA_ATTR uint16_t  seq_id_counter = 0;
RTC_DATA_ATTR uint32_t  wkup_count = 0;

uint16_t mVolts;
uint8_t  err = 0;

void setup() {
  
  setCpuFrequencyMhz( CPU_FREQ_LOW_MHZ);
  wkup_count++;
  // Cold boot / reset / brownout: RTC memory was wiped. Reseed seq_id so it
  // stays near-globally-unique across resets (not just within one session).
  if( esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED) {
    seq_id_counter = esp_random() & 0xFFFF;
  }
  init_time = millis();
  #ifdef DEBUG
    Serial.begin(115200);
    delay(4000);
  #endif
  LOG2("CpuFrequencyMhz: ", getCpuFrequencyMhz())
  Wire.setPins( SDA_PIN, SCL_PIN);
  Wire.begin();

  if( !bme.begin( BME280_ADDR)) {
    LOG("ERROR BME280 sensor not found.")
    delay( BME280_RETRY_DELAY_MS);
    bme.begin( BME280_ADDR);
  }
  bme.setSampling(
    Adafruit_BME280::MODE_FORCED,
    Adafruit_BME280::SAMPLING_X1, // temperature
    Adafruit_BME280::SAMPLING_X1, // pressure
    Adafruit_BME280::SAMPLING_X1, // humidity
    Adafruit_BME280::FILTER_OFF );
}

void loop() {
  // --- Take a sample -----------------------------------------------
  mVolts = analogReadMilliVolts( ADC_VOLT_PIN) * ADC_MULTIPLIER + ADC_OFFSET_MV;
  bme.takeForcedMeasurement();

  // Buffer overflow protection: drop oldest if full
  if( sample_count_total >= max_samples) {
    memmove( samples, samples + 1, ( max_samples - 1) * sizeof( Sample));
    sample_count_total = max_samples - 1;
    LOG("Sample buffer full, dropped oldest")
  }

  samples[ sample_count_total].temp      = int16_t( bme.readTemperature() * 10 + 0.5);
  samples[ sample_count_total].hum       = uint8_t( bme.readHumidity() * 2 + 0.5);
  samples[ sample_count_total].pres      = uint8_t(( bme.readPressure() / 100 - 950) * 2 + 0.5);
  samples[ sample_count_total].volts     = mVolts;
  samples[ sample_count_total].sleep_mins = sleep_mins;
  samples[ sample_count_total].seq_id    = ++seq_id_counter;

  LOG("+-+-+-+-+-+-+-+-+")
  LOG2("Sample:", sample_count_total)
  LOG2("seq_id:", samples[ sample_count_total].seq_id)
  LOG2("temp:",   samples[ sample_count_total].temp / 10.0F)
  LOG2("hum:",    samples[ sample_count_total].hum / 2.0F)
  LOG2("pres:",   samples[ sample_count_total].pres / 2.0F + 950)
  LOG2("mVolts:", mVolts)
  LOG2("Sleep:",  samples[ sample_count_total].sleep_mins)
  LOG("+-+-+-+-+-+-+-+-+")
  LOG("")

  sample_count_total++;
  sample_count_new++;

  // --- Decide whether to transmit ----------------------------------
  sleep_mins = ( mVolts < MVOLT_CRITICAL) ? LONG_SLEEP_MIN : SHORT_SLEEP_MIN;
  samples_to_send = ( mVolts < MVOLT_LOW_THRESHOLD) ? PACK_SIZE_LOW_VOLTAGE : PACK_SIZE_NORMAL;

  bool should_send = ( mVolts >= MVOLT_CRITICAL) &&
                     ( ( mVolts >= MVOLT_HIGH_SEND_THRESHOLD && sample_count_new % 2 == 0) ||
                       sample_count_new >= samples_to_send );

  if( should_send) {
    setCpuFrequencyMhz( CPU_FREQ_HIGH_MHZ);
    err = connectWifi();
    if( err != CONN_OK) {
      samples[ sample_count_total - 1].error = err;
    } else {
      err = connectAWS( wifiClient, awsClient);
      if( err != CONN_OK) {
        samples[ sample_count_total - 1].error = err;
      } else {
        // Send newest-first. AWS deduplicates by seq_id

        // Repair RTC-corrupted sleep_mins on a first pass
        for( uint16_t i = 0; i < sample_count_total; i++) {
          if( samples[i].sleep_mins != SHORT_SLEEP_MIN &&
              samples[i].sleep_mins != LONG_SLEEP_MIN)
            samples[i].sleep_mins = SHORT_SLEEP_MIN;
        }

        // newer_sleep = sum of sleep_mins of all already-sent (newer) samples
        uint16_t newer_sleep = 0;
        uint16_t sent_count = 0;
        bool all_ok = true;
        for( int16_t i = sample_count_total - 1; i >= 0; i--) {
          samples[i].error = 0;
          err = send_iot_data( samples[i], newer_sleep, awsClient);
          if( err != CONN_OK) {
            samples[i].error = err;
            LOG2("Error sending data:", i);
            all_ok = false;
            break;
          }
          newer_sleep += samples[i].sleep_mins;
          sent_count++;
          if( sent_count % MQTT_LOOP_INTERVAL == 0) awsClient.loop();
        }

        if( all_ok) {
          uint16_t keep = ( sample_count_total > samples_to_send) ? samples_to_send : sample_count_total;
          if( keep < sample_count_total) {
            memmove( samples, samples + ( sample_count_total - keep), keep * sizeof( Sample));
            sample_count_total = keep;
          }
          sample_count_new = 0;
        }

        delay( AWS_DISCONNECT_DELAY_MS);
        awsClient.disconnect();
      }
      WiFi.mode( WIFI_OFF);
    }
  }

  // delay(60 * 1000);
  esp_deep_sleep( ( sleep_mins * 60UL * 1000UL - ( millis() - init_time)) * 1000UL);
}
