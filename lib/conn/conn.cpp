#include "conn.h"
#include "conf.h"
#include "../env.h"

// Static ip instead of DHCP, to save time/power 
IPAddress ip( IP);
IPAddress dns( DNS);
IPAddress gateway( GATEWAY);
IPAddress subnet( SUBNET);

uint8_t connectWifi()
{
  WiFi.useStaticBuffers( true);
  WiFi.mode( WIFI_STA);
  if (!WiFi.config( ip, gateway, subnet, dns)) {
    LOG("Failed to configure WIFI");
    return ERR_CONF_WIFI;
  }
  WiFi.begin( WIFI_SSID, WIFI_PASSWORD); 
  LOG2( "Attempting to connect to SSID: ", WIFI_SSID)
  while (WiFi.status() != WL_CONNECTED){
    LOG(".");
    delay(400);
  }
  if( WiFi.status() != WL_CONNECTED) return ERR_CONNECTING_WIFI;
  LOG("")
  LOG("WiFi connected")
  LOG2("IP address: ", WiFi.localIP())
  return CONN_OK;
}


uint8_t connectAWS( WiFiClientSecure& wifiClient, PubSubClient& awsClient)
{
  wifiClient.setCACert( AWS_CERT_CA); // TODO: move to setup()
  wifiClient.setCertificate( AWS_CERT_CRT); // for client verification
  wifiClient.setPrivateKey( AWS_CERT_PRIVATE);	// for client verification
  awsClient.setClient( wifiClient);
  awsClient.setServer( AWS_MQTT_HOST, 8883);
  // client.setCallback( messageReceived);
 
  LOG("Connecting to AWS IOT");
  
  for( int i=0; i < 5; i++){ // try 5 times
    if( awsClient.connect( THINGNAME)) break;
    char err_buf[256];
    wifiClient.lastError( err_buf, sizeof( err_buf));
    LOG2( err_buf, awsClient.state())
    delay(400);
  }
 
  if ( !awsClient.connected()) {
    LOG("AWS IoT Timeout!")
    return ERR_CONNECTING_AWS;
  }
  // Subscribe to a topic
  // client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  LOG("AWS IoT Connected!")
  return CONN_OK;
}


// void NTPConnect()
// {
//   LOG("Setting time using SNTP")
//   configTzTime("CET-1CEST,M3.5.0/2,M10.5.0/3", "pool.ntp.org", "time.nist.gov"); // CET with daylight saving
//   tm rtcTime;
//   getLocalTime( &rtcTime);
//   char buf[40];
//   strftime( buf, sizeof(buf), "%d-%b-%Y %T %W/%w %Z %z", &rtcTime); // 2019-01-15 16:33:20 02/2 MEZ +0100	
//   LOG2("Current time: ", buf)
// }

// time formatting options:
// https://cplusplus.com/reference/ctime/strftime/?kw=strftime

  // struct tm
  // {
  //   int	tm_sec;   // 0..59
  //   int	tm_min;   // 0..59
  //   int	tm_hour;  // 0..23 hours since midnight
  //   int	tm_mday;  // 1..31 day of month
  //   int	tm_mon;   // 0..11 January=0 .. December=11
  //   int	tm_year;  // years since 1900
  //   int	tm_wday;  // 0..6  weekday, Sunday=0 .. Saturday=6
  //   int	tm_yday;  // 0..365 day in the year, January 1 = 0
  //   int	tm_isdst; //  0: Standard time, 
  //                 // >0: Daylight saving time, 
  //                 // <0: information not available 
  // };