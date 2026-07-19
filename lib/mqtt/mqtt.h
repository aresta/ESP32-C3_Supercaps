#ifndef mqtt_
#define mqtt_

#include <PubSubClient.h>
#include "../data/data.h"

#define AWS_IOT_PUBLISH_TOPIC "balco/sensor/data"
#define AWS_IOT_PUBLISH_LOG_INFO "balco/sensor/log/info"
#define AWS_IOT_PUBLISH_LOG_ERROR "balco/sensor/log/error"

/// @brief Publish one sample to AWS IoT
/// @param sample   the sample to send (includes seq_id)
/// @param offset_mins  minutes between this sample's capture and now
/// @return error code
uint8_t send_iot_data( const Sample& sample, uint16_t offset_mins, PubSubClient& awsClient);

#endif