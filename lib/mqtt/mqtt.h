#ifndef mqtt_
#define mqtt_

#define AWS_IOT_PUBLISH_TOPIC "balco/sensor/data"
#define AWS_IOT_PUBLISH_LOG_INFO "balco/sensor/log/info"
#define AWS_IOT_PUBLISH_LOG_ERROR "balco/sensor/log/error"

/// @brief 
/// @return error code
u_int8_t send_iot_data( const char volts[], const char hum[], const char pres[], const char temp[], const char offset_mins[], uint32_t wkup_cnt, PubSubClient& awsClient);
// void log_iot( int channel, const char msg[]);

#endif