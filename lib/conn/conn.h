#ifndef conn_
#define conn_
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const int8_t TIME_ZONE = 1;

void connectWifi();
void connectAWS( WiFiClientSecure& wifiClient, PubSubClient& awsClient);
// void NTPConnect();

#endif