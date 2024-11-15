#include <Arduino.h>
#include <Adafruit_BME280.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "WiFi.h"
#include <Wire.h>
#include "data.h"
#include "conn.h"
#include "mqtt.h"
#include "conf.h"

#define LED GPIO_NUM_8
// #define SDA_PIN 7
// #define SCL_PIN 10
// #define ADC_VOLT_PIN 3
#define SCL_PIN 5
#define SDA_PIN 6
#define ADC_VOLT_PIN 3

Adafruit_BME280 bme;
WiFiClientSecure wifiClient;
PubSubClient awsClient;

int32_t init_time = 0;
#define SHORT_SLEEP 15
#define LONG_SLEEP 30
RTC_DATA_ATTR Sample    samples[ max_samples];
RTC_DATA_ATTR uint16_t  sample_count = 0;
RTC_DATA_ATTR uint16_t  sleep_mins = SHORT_SLEEP;
RTC_DATA_ATTR uint8_t   samples_to_send = 8;
RTC_DATA_ATTR uint32_t  wkup_count = 0;

uint16_t mVolts;
uint8_t err = 0;

void setup() {
  
  setCpuFrequencyMhz(10);
  wkup_count++;
  init_time = millis();
  #ifdef DEBUG
    Serial.begin(115200);
    delay(4000);
  #endif
  LOG2("CpuFrequencyMhz: ", getCpuFrequencyMhz())
  Wire.setPins( SDA_PIN, SCL_PIN);
  Wire.begin();

  if( !bme.begin( 0x76)) { // 0x77 per l'altre sensor
    LOG("ERROR BME280 sensor not found.")
    delay(1000);
    bme.begin( 0x76); // try again
  }
  bme.setSampling(
    Adafruit_BME280::MODE_FORCED,
    Adafruit_BME280::SAMPLING_X1, // temperature
    Adafruit_BME280::SAMPLING_X1, // pressure
    Adafruit_BME280::SAMPLING_X1, // humidity
    Adafruit_BME280::FILTER_OFF );
}

void loop() {
  mVolts = analogReadMilliVolts( ADC_VOLT_PIN) * 2.14 + 20;
  bme.takeForcedMeasurement();
  samples[ sample_count].temp = int16_t( bme.readTemperature()*10 + 0.5);         // range: -50c - +50c => -500 - +500 
  samples[ sample_count].hum  = uint8_t( bme.readHumidity()*2 + 0.5);             // range: 0% - 100% => 0 - 200
  samples[ sample_count].pres = uint8_t(( bme.readPressure()/100 - 950)*2 + 0.5); // range: 95000 - 105000 => 0 - 200
  samples[ sample_count].volts = mVolts; 
  samples[ sample_count].sleep_mins = sleep_mins;

  LOG("+-+-+-+-+-+-+-+-+")
  LOG2("Sample:", sample_count)
  LOG2("temp:", samples[ sample_count].temp/10.0F)
  LOG2("hum:", samples[ sample_count].hum/2.0F)
  LOG2("pres:", samples[ sample_count].pres/2.0F + 950)
  LOG2("mVolts:", mVolts)
  LOG2("Sleep:", samples[ sample_count].sleep_mins)
  LOG("+-+-+-+-+-+-+-+-+")
  LOG("")

  char temp[10], hum[10], pres[10], volts[10], sleep_mins_[10];
  sample_count++;
  if( mVolts >= 5000 || // above this voltage send always
      sample_count >= samples_to_send ){
    setCpuFrequencyMhz(80);
    err = connectWifi();
    if( err != CONN_OK){
      samples[ sample_count-1 ].error = err;
    } else {
      // NTPConnect();
      err = connectAWS( wifiClient, awsClient);
      if( err != CONN_OK){
        samples[ sample_count-1 ].error = err;
      } else {
        uint16_t offset_mins = sample_count * sleep_mins;
        for( int i=0; i < sample_count; i++){
          offset_mins -= samples[i].sleep_mins;
          sprintf( temp,    "%.1f", samples[i].temp/10.0F);
          sprintf( hum,     "%.1f", samples[i].hum/2.0F);
          sprintf( pres,    "%.1f", samples[i].pres/2.0F + 950);
          sprintf( volts,   "%.3f", samples[i].volts/1000.0F);
          sprintf( sleep_mins_, "%i", offset_mins);
          // err = send_iot_data( volts, hum, pres,  temp, sleep_mins_, samples[i].error, awsClient);
          err = send_iot_data( volts, hum, pres,  temp, sleep_mins_, wkup_count, awsClient);
          if( err != CONN_OK) {
            samples[i].error = err;
            LOG2("Error sending data:", i);
            break;  // TODO: keep only remaining samples to send
          }
        }
        if( err == CONN_OK){
          sample_count = 0;
        }
        delay(120);
        awsClient.disconnect();
        // WiFi.disconnect( true);
      }
      WiFi.mode( WIFI_OFF);    // Switch WiFi off
    }
  }

  if( mVolts < 3600){
    samples_to_send = 16;
  } else {
    samples_to_send = 8;
  }
  // if( mVolts < mVOLTS(3.5)){  // below 3.5V
  //   sleep_mins = LONG_SLEEP;
  // } else {
  //   sleep_mins = SHORT_SLEEP;
  // }

  // delay( 1*60*1000);
  esp_deep_sleep(( sleep_mins*60*1000 - (millis() - init_time))* 1000); // microseconds to sleep
}
