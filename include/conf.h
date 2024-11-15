#ifndef conf_
#define conf_

// uncomment to enable log output
// #define DEBUG

#ifdef DEBUG
#define LOG( l) { Serial.println(l);}
#define LOG2( l1, l2) { Serial.print(l1); Serial.println(l2);}
#else
#define LOG(l) // nothing
#define LOG2( l1, l2) // nothing
#endif

#define CONN_OK 0
#define ERR_CONF_WIFI 1
#define ERR_CONNECTING_WIFI 2
#define ERR_CONNECTING_AWS 3
#define ERR_SENDING_AWS 4

#endif