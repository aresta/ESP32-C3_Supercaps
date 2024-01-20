#ifndef mqtt_
#define mqtt_

#define AWS_IOT_PUBLISH_TOPIC "balco/sensor/data"
#define AWS_IOT_PUBLISH_LOG_INFO "balco/sensor/log/info"
#define AWS_IOT_PUBLISH_LOG_ERROR "balco/sensor/log/error"

void send_iot_data( const char volts[], const char hum[], const char pres[], const char temp[], const char offset_mins[], PubSubClient& awsClient);
void log_iot( int channel, const char msg[]);

#endif