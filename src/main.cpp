#define MAJOR_VER "00"
#define MINOR_VER "03"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <time.h>
#include "AsyncWait.h"
#include "enums.h"
#include "config.h"
#include "SetupWifi.h"

// TODO: implement secure credentials as a runtime config file
//       rather than a header file.
#include "certs.h"

SetupWifi setupWifi(
    STASSID, STAPSK,
    CA_CERT_PROG, CLIENT_CERT_PROG, CLIENT_KEY_PROG);

const char *mqtt_server = MQTT_SERVER;
// IPAddress broker(192,168,1,1); // IP address of your MQTT broker

const char *ID = "sec_mqtt_client_" MAJOR_VER "_" MINOR_VER; // Name of our device, must be unique
const String TOPIC_ZONE_ON("irrigation/zone/on");
const String TOPIC_ZONE_OFF("irrigation/zone/off");
const String TOPIC_ZONE_STATUS("irrigation/zone/status");

static PubSubClient pubsubClient(setupWifi.getWiFiClient());

// Handle incomming messages from the broker
void callback(char *topic, byte *payload, unsigned int length)
{
  String topicStr;
  String payloadStr;

  for (int i = 0; topic[i]; i++)
  {
    topicStr += topic[i];
  }

  for (unsigned int i = 0; i < length; i++)
  {
    payloadStr += (char)payload[i];
  }

  DEBUG_LOGLN("");
  DEBUG_LOG("Message arrived - [");
  DEBUG_LOG(topicStr);
  DEBUG_LOG("] ");
  DEBUG_LOGLN(payloadStr);
}

// Reconnect to the MQTT client.
void reconnectToMQTT(MilliSec currentMilliSec)
{
  if (pubsubClient.connected())
  {
    // We are connected so nothing further needs to be done.
    return;
  }

  static AsyncWait waitToRetry;
  if (waitToRetry.isWaiting(currentMilliSec))
  {
    return;
  }

  DEBUG_LOG("Attempting MQTT connection...");
  // Attempt to connect
  if (pubsubClient.connect(ID, MQTT_USER, MQTT_PASSWORD))
  {
    DEBUG_LOGLN("connected");

    pubsubClient.subscribe(TOPIC_ZONE_ON.c_str());
    DEBUG_LOG("Subcribed to: ");
    DEBUG_LOGLN(TOPIC_ZONE_ON);

    pubsubClient.subscribe(TOPIC_ZONE_OFF.c_str());
    DEBUG_LOG("Subcribed to: ");
    DEBUG_LOGLN(TOPIC_ZONE_OFF);

    pubsubClient.subscribe(TOPIC_ZONE_STATUS.c_str());
    DEBUG_LOG("Subcribed to: ");
    DEBUG_LOGLN(TOPIC_ZONE_STATUS);
  }
  else
  {
    DEBUG_LOGLN(F("[MQTT] failed, rc="));
    DEBUG_LOG(pubsubClient.state());
    DEBUG_LOGLN(" try again in 5 seconds.");
    waitToRetry.startWaiting(currentMilliSec, 5000);
  }
}

void setup()
{
#ifdef DEBUG
  Serial.begin(115200); // Start serial communication at 115200 baud
#endif

  setupWifi.setupWifi();
  // pubsubClient.setServer(broker, 1883);
  pubsubClient.setServer(mqtt_server, 8883);
  pubsubClient.setCallback(callback); // Initialize the callback routine
  DEBUG_LOGLN("Setup");
}

#ifdef DEBUG
void startupTest(MilliSec currentMilliSec)
{
  static AsyncWait startupTestWait;
  static uint8_t startupTestValue;
  static bool firstTime = true;
  static const unsigned turnOnSeconds = 1;            // 1 second.
  static const unsigned nextIterationDuration = 1250; // 1,250 milliseconds.

  bool changed = false;
}
#endif // DEBUG

void loop()
{
  setupWifi.loopWifi();
  if (!setupWifi.isReadyForProcessing())
  {
    // The WiFi is not ready yet so
    // don't do any further processing.
    return;
  }

  if (!pubsubClient.connected())
  {
    // Reconnect if connection is lost.
    MilliSec currentMilliSec = millis();
    reconnectToMQTT(currentMilliSec);
  }
  pubsubClient.loop();

  { // App code.
    MilliSec currentMilliSec = millis();

#ifdef DEBUG
    startupTest(currentMilliSec);
#endif
  }
}