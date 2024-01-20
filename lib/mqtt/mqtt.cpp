
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "mqtt.h"

void send_iot_data( const char volts[], const char hum[], const char pres[], const char temp[], const char offset_mins[], PubSubClient& awsClient)
{
  JsonDocument doc;
  doc["temp"] = temp;
  doc["hum"] = hum;
  doc["presio"] = pres;
  doc["volts"] = volts;
  doc["offset"] = offset_mins;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
  awsClient.publish( AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
  delay(100);
}


void log_iot( int channel, const char msg[])
{
  JsonDocument doc;
  // doc["channel"]  = channel;
  doc.add( msg );
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
//   connectAWS();
//   client.publish( (channel == CHANNEL_ERROR ? AWS_IOT_PUBLISH_LOG_ERROR : AWS_IOT_PUBLISH_LOG_INFO), jsonBuffer);
  delay(200);
}