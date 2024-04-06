#ifndef conn_
#define conn_
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const int8_t TIME_ZONE = 1;

/// @brief 
/// @return error code
uint8_t connectWifi();

/// @brief 
/// @return error code
uint8_t connectAWS( WiFiClientSecure& wifiClient, PubSubClient& awsClient);
// void NTPConnect();

#endif