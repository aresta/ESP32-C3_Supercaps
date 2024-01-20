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

#endif