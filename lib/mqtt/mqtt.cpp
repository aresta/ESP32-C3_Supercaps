
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "mqtt.h"
#include "conf.h"

uint8_t send_iot_data( const Sample& sample, uint16_t offset_mins, PubSubClient& awsClient)
{
  JsonDocument doc;
  doc["temp"]    = sample.temp / 10.0;
  doc["hum"]     = sample.hum / 2.0;
  doc["presio"]  = sample.pres / 2.0 + 950;
  doc["volts"]   = sample.volts / 1000.0;
  doc["off_min"] = offset_mins;
  doc["seq_id"]  = sample.seq_id;

  size_t needed = measureJson( doc) + 1;
  char jsonBuffer[needed];
  serializeJson( doc, jsonBuffer, needed);

  bool ok = awsClient.publish( AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
  delay(MQTT_PUBLISH_DELAY_MS);
  if( !ok) return ERR_SENDING_AWS;
  return CONN_OK;
}
