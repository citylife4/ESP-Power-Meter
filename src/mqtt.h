#ifndef TASK_MQTT_AWS
#define TASK_MQTT_AWS

#include "config.h"
#include "certs.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include <PubSubClient.h>
#include <MQTT.h>

extern float measurements[];

#define MQQT_MAX_MSG_SIZE_BYTES 300

WiFiClientSecure net;
BearSSL::X509List *serverCaList = new BearSSL::X509List(server_ca);
BearSSL::X509List *clienCertList = new BearSSL::X509List(client_cert);
BearSSL::PrivateKey *serverPrivKey = new BearSSL::PrivateKey(client_key);

MQTTClient client;

// Just for debug
// For now
void subscribeReceive(char *topic, byte *payload, unsigned int length)
{
    // Print the topic
    Serial.print("Topic: ");
    Serial.println(topic);

    // Print the message
    Serial.print("Message: ");
    for (int i = 0; i < length; i++)
    {
        Serial.print(char(payload[i]));
    }

    // Print a newline
    Serial.println("");
}

void connectToMqtt(bool nonBlocking = false)
{
  Serial.print("MQTT connecting ");
  while (!client.connected())
  {
    if (client.connect(THINGNAME))
    {
      Serial.println("connected!");
      if (!client.subscribe(MQTT_SUB_TOPIC))
        lwMQTTErr(client.lastError());
    }
    else
    {
      Serial.print("SSL Error Code: ");
      Serial.println(net.getLastSSLError());
      Serial.print("failed, reason -> ");
      lwMQTTErrConnection(client.returnCode());
      if (!nonBlocking)
      {
        Serial.println(" < try again in 5 seconds");
        delay(5000);
      }
      else
      {
        Serial.println(" <");
      }
    }
    Serial.printf("Max stack %d.", stack_thunk_get_max_usage());
    if (nonBlocking)
      break;
  }
}

void keepMQTTConnectionAlive()
{

    if (client.connected())
    {
        client.loop();
        return;
    }

    if (!WiFi.isConnected())
    {
        return;
    }

    // Configure certificates
    // MQQT_net.setInsecure();

    net.setTrustAnchors(serverCaList);
    net.setClientRSACert(clienCertList, serverPrivKey);
    serial_println(F("[MQTT] Connecting to broker..."));
    client.begin(MQTT_HOST, MQTT_PORT, net);
    client.onMessage(subscribeReceive);
    ;

    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD, MQTT_TOPIC_STATE, 1, true, "disconnected", false))
    {
        serial_println(F("[MQTT] connected"));

        // Once connected, publish an announcement...
        client.publish(MQTT_TOPIC_STATE, "connected", true);
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
        // serial_println("[MQTT] AWS: no connection. Discarding data..");
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
    // vTaskDelete(NULL);
}
#endif