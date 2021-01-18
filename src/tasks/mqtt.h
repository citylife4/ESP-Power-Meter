#ifndef TASK_MQTT_AWS
#define TASK_MQTT_AWS

#include "config.h"
#include "config_sen.h" 

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

extern float measurements[];

#define MQQT_MAX_MSG_SIZE_BYTES 300

WiFiClientSecure MQQT_net;
PubSubClient MQQT_client(MQQT_net);

// Just for debug 
// For now
void subscribeReceive(char* topic, byte* payload, unsigned int length)
{
  // Print the topic
  Serial.print("Topic: ");
  Serial.println(topic);
 
  // Print the message
  Serial.print("Message: ");
  for(int i = 0; i < length; i ++)
  {
    Serial.print(char(payload[i]));
  }
 
  // Print a newline
  Serial.println("");
}

void keepMQTTConnectionAlive()
{

    if (MQQT_client.connected())
    {
        MQQT_client.loop();
        return;
    }

    if (!WiFi.isConnected())
    {
        return;
    }

    // Configure certificates
    MQQT_net.setInsecure();
    serial_println(F("[MQTT] Connecting to broker..."));
    MQQT_client.setServer(MQTT_SERVER, 8883);
    MQQT_client.setCallback(subscribeReceive);
    ;

    // Attempt to connect
    if (MQQT_client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD, MQTT_TOPIC_STATE, 1, true, "disconnected", false))
    {
        serial_println(F("[MQTT] connected"));

        // Once connected, publish an announcement...
        MQQT_client.publish(MQTT_TOPIC_STATE, "connected", true);
    }
    else
    {
        serial_print(F("[MQTT] failed, rc="));
        serial_print(MQQT_client.state());
        serial_println(F(" try again in 5 seconds"));
        delay(5000);
    }
}

/**
 * TASK: Upload measurements to AWS. This only works when there are enough
 * local measurements. It's called by the measurement function.
 */
void uploadMeasurementsToAWS()
{
    if (!WiFi.isConnected() || !MQQT_client.connected())
    {
        serial_println("[MQTT] AWS: no connection. Discarding data..");
        return;
    }

    char msg[MQQT_MAX_MSG_SIZE_BYTES];
    strcpy(msg, "{\"readings\":[");

    for (short i = 0; i < LOCAL_MEASUREMENTS - 1; i++)
    {
        strcat(msg, String(measurements[i]).c_str());
        strcat(msg, ",");
    }

    strcat(msg, String(measurements[LOCAL_MEASUREMENTS - 1]).c_str());
    strcat(msg, "]}");

    serial_print("[MQTT] AWS publish: ");
    serial_println(msg);
    MQQT_client.publish("bla", msg);

    // Task is done!
    //vTaskDelete(NULL);
}
#endif