
#define AP_USERNAME 
#define AP_PASSWORD 


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>

#include "ESP_sensitive.ino"

// Enable debug prints
#define MY_DEBUG

#define MQTT_TOPIC_KWH "home/energymeter/kwh"
#define MQTT_TOPIC_WATT "home/energymeter/watt"
#define MQTT_TOPIC_PULSES "home/energymeter/pulses"
#define MQTT_TOPIC_STATE "home/energymeter/status"
#define MQTT_PUBLISH_DELAY 200
#define MQTT_CHECK_DELAY 2
#define MQTT_CLIENT_ID "esp8266bme280"

#define BME280_ADDRESS 0x76

#define DIGITAL_INPUT_SENSOR D1  // The digital input you attached your light sensor.  (Only 2 and 3 generates interrupt!)
#define PULSE_FACTOR 1000       // Number of blinks per kWh of your meter. Normally 1000.
#define SLEEP_MODE false        // Watt value can only be reported when sleep mode is false.
#define MAX_WATT 10000          // Max watt value to report. This filters outliers.
#define CHILD_ID 1              // Id of the sensor child

// Replace the following your sensitive info

// TLS
// static const char *fingerprint PROGMEM = XXXX ;

//MQTT
//const char *MQTT_SERVER = "XXXXXX";
//const char *MQTT_USER = "XXXXXX"; // NULL for no authentication
//const char *MQTT_PASSWORD = "XXXXXXX"; // NULL for no authentication

//NETWORK
//const char* ssid = "XXXX";
//const char* password = "XXXX";

const int ESP_BUILTIN_LED = 2;
bool check;

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

uint32_t SEND_FREQUENCY =
    5000; // Minimum time between send (in milliseconds). We don't want to spam the gateway.
double ppwh = ((double)PULSE_FACTOR) / 1000; // Pulses per watt hour
bool pcReceived = false;
volatile uint32_t pulseCount = 0;
volatile uint32_t lastBlinkmicros = 0;
volatile uint32_t lastBlinkmillis = 0;
volatile uint32_t watt = 0;
uint32_t oldPulseCount = 0;
uint32_t oldWatt = 0;
double oldkWh;
uint32_t lastSend;
bool current = LOW;

const int DEBOUNCE_DELAY = 50;   // the debounce time; increase if the output flickers

// Variables will change:
uint32_t lastSteadyState = LOW;       // the previous steady state from the input pin
uint32_t lastFlickerableState = LOW;  // the previous flickerable state from the input pin
uint32_t currentState;                // the current reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled

//TODO: try with an Hardware debouncer

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
#define IRQ_HANDLER_ATTR ICACHE_RAM_ATTR
#else
#define IRQ_HANDLER_ATTR
#endif

void IRQ_HANDLER_ATTR onPulse()
{
    if (!SLEEP_MODE) {
        uint32_t newBlinkmicros = micros();
        uint32_t newBlinkmillis = millis();
        uint32_t intervalmicros = newBlinkmicros - lastBlinkmicros;
        uint32_t intervalmillis = newBlinkmillis - lastBlinkmillis;
        if (intervalmicros < 100000 || intervalmillis < 100) { // Sometimes we get interrupt on RISING
            return;
        }
        if (intervalmillis < 360000) { // Less than an hour since last pulse, use microseconds
            watt = (3600000000.0 / intervalmicros) / ppwh;
        } else {
            watt = (3600000.0 / intervalmillis) /
                   ppwh; // more thAn an hour since last pulse, use milliseconds as micros will overflow after 70min
        }
        lastBlinkmicros = newBlinkmicros;
        lastBlinkmillis = newBlinkmillis;
    }
    pulseCount++;
    //mqttPublish(MQTT_TOPIC_TEMPERATURE, pulseCount);
}

boolean debounce_ldr()
{
// read the state of the switch/button:
  currentState = digitalRead(DIGITAL_INPUT_SENSOR);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch/button changed, due to noise or pressing:
  if (currentState != lastFlickerableState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
    // save the the last flickerable state
    lastFlickerableState = currentState;
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if(lastSteadyState == HIGH && currentState == LOW) {
      pulseCount++;
      mqttPublish(MQTT_TOPIC_PULSES, pulseCount);
    }

    // save the the last steady state
    lastSteadyState = currentState;
  }
}


void mqttReconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD, MQTT_TOPIC_STATE, 1, true, "disconnected", false)) {
      Serial.println("connected");

      // Once connected, publish an announcement...
      mqttClient.publish(MQTT_TOPIC_STATE, "connected", true);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqttPublish(char *topic, float payload) {
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(payload);

  mqttClient.publish(topic, String(payload).c_str(), true);
}

void OTAconfig() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
    
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  
  WiFi.begin(ssid, password);
//  WiFi.config(staticIP, gateway, subnet);
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // espClient.setFingerprint(fingerprint);   // <-- Change #3: Set the SHA1 fingerprint
  espClient.setInsecure();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  OTAconfig();

  mqttClient.setServer(MQTT_SERVER, 8883);
  mqttClient.setCallback(subscribeReceive);
  //pinMode(ESP_BUILTIN_LED, OUTPUT);

  // Fetch last known pulse count value from gw
  //request(CHILD_ID, V_VAR1);

  // Use the internal pullup to be able to hook up this sketch directly to an energy meter with S0 output
  // If no pullup is used, the reported usage will be too high because of the floating pin
  pinMode(DIGITAL_INPUT_SENSOR, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(DIGITAL_INPUT_SENSOR), onPulse, RISING);
  lastSend = millis();


}

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

void loop() {
  
  ArduinoOTA.handle();

  if (!mqttClient.connected()) {
      mqttReconnect();
  }
  mqttClient.loop();
  //debounce_ldr();

  uint32_t now = millis();
  // Only send values at a maximum frequency or woken up from sleep
  bool sendTime = now - lastSend > SEND_FREQUENCY;
  if ((SLEEP_MODE || sendTime))
  {
    // New watt value has been calculated
    //if (!SLEEP_MODE && watt != oldWatt)
    if (!SLEEP_MODE && watt != oldWatt)
    {
      // Check that we don't get unreasonable large watt value, which
      // could happen when long wraps or false interrupt triggered
      if (watt < ((uint32_t)MAX_WATT))
      {
        mqttPublish(MQTT_TOPIC_WATT, watt);
        //send(wattMsg.set(watt));  // Send watt value to gw
        lastSend = now;
      }
      Serial.print("Watt:");
      Serial.println(watt);
      oldWatt = watt;
    }

    // Pulse count value has changed
    if (pulseCount != oldPulseCount)
    {
      //send(pcMsg.set(pulseCount));  // Send pulse count value to gw
      double kWh = ((double)pulseCount / ((double)PULSE_FACTOR));
      oldPulseCount = pulseCount;
      if (kWh != oldkWh)
      {
        //send(kWhMsg.set(kWh, 4));  // Send kWh value to gw
        mqttPublish(MQTT_TOPIC_KWH, kWh);
        oldkWh = kWh;
      }
      mqttPublish(MQTT_TOPIC_PULSES, pulseCount);
    }
    //} else if (sendTime && !pcReceived) {
  }
  /*
  else if (sendTime)
  {
    // No pulse count value received from controller. Try requesting it again.
    //request(CHILD_ID, V_VAR1);
    lastSend = now;
  }

  long now = millis();
  if (now - lastCheckTime > MQTT_CHECK_DELAY) {
    lastCheckTime = now;

    // Reading BME280 sensor data
      int sensorValue = analogRead(SENSOR_PIN);
      float level = 100 - ((sensorValue - MIN_VALUE) * 100 / (MAX_VALUE - MIN_VALUE));
      // Publishing sensor data
      if (level>90 && now - lastMsgTime > MQTT_PUBLISH_DELAY) {
        lastMsgTime = now;
        mqttPublish(MQTT_TOPIC_TEMPERATURE, level);
      }
     // mqttPublish(MQTT_TOPIC_HUMIDITY, humidity);
  }

  digitalWrite(ESP_BUILTIN_LED, LOW);
  delay(2200);
  digitalWrite(ESP_BUILTIN_LED, HIGH);
  delay(2000);
  */
}