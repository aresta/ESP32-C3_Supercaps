#ifndef conf_
#define conf_

// uncomment to enable log output
#define DEBUG

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

// --- Sleep intervals (minutes) ---
#define SHORT_SLEEP_MIN 15
#define LONG_SLEEP_MIN  30

// --- Pack sizes (samples accumulated before a transmit) ---
#define PACK_SIZE_NORMAL       8
#define PACK_SIZE_LOW_VOLTAGE 16

// --- Voltage thresholds (millivolts) ---
#define MVOLT_CRITICAL             3500  // below this: no WiFi, long sleep
#define MVOLT_LOW_THRESHOLD        4200  // below this: larger pack size
#define MVOLT_HIGH_SEND_THRESHOLD  4900  // above this: send every 2 samples

// --- ADC voltage divider calibration ---
#define ADC_MULTIPLIER 2.14
#define ADC_OFFSET_MV  20

// --- BME280 ---
#define BME280_ADDR          0x76
#define BME280_RETRY_DELAY_MS 1000

// --- Connectivity timeouts ---
#define WIFI_CONNECT_TIMEOUT_MS  12000
#define WIFI_RETRY_DELAY_MS        400
#define AWS_CONNECT_RETRIES          5
#define AWS_CONNECT_DELAY_MS       400
#define MQTT_PUBLISH_DELAY_MS      100
#define AWS_DISCONNECT_DELAY_MS    120

// --- CPU frequencies ---
#define CPU_FREQ_LOW_MHZ  10
#define CPU_FREQ_HIGH_MHZ 80

#endif