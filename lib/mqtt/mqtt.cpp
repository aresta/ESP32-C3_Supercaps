
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "mqtt.h"
#include "conf.h"

u_int8_t send_iot_data( const char volts[], const char hum[], const char pres[], const char temp[], const char offset_mins[], uint32_t wkup_cnt, PubSubClient& awsClient)
{
  JsonDocument doc;
  doc["temp"] = temp;
  doc["hum"] = hum;
  doc["presio"] = pres;
  doc["volts"] = volts;
  doc["offset"] = offset_mins;
  doc["wkup_cnt"] = wkup_cnt;
  // switch ( error)
  // {
  //   case ERR_CONF_WIFI:
  //     doc["error"] = "ERR_CONF_WIFI";
  //     break;
  //   case ERR_CONNECTING_WIFI:
  //     doc["error"] = "ERR_CONNECTING_WIFI";
  //     break;
  //   case ERR_CONNECTING_AWS:
  //     doc["error"] = "ERR_CONNECTING_AWS";
  //     break;
  //   case ERR_SENDING_AWS:
  //     doc["error"] = "ERR_SENDING_AWS";
  //     break;
  //   default:
  //     doc["error"] = "";
  //     break;
  // }

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
  bool ok = awsClient.publish( AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
  delay(100);
  if( !ok) return ERR_SENDING_AWS;
  return CONN_OK;
}


// void log_iot( int channel, const char msg[])
// {
//   JsonDocument doc;
//   // doc["channel"]  = channel;
//   doc.add( msg );
//   char jsonBuffer[512];
//   serializeJson(doc, jsonBuffer);
// //   connectAWS();
// //   client.publish( (channel == CHANNEL_ERROR ? AWS_IOT_PUBLISH_LOG_ERROR : AWS_IOT_PUBLISH_LOG_INFO), jsonBuffer);
//   delay(200);
// }