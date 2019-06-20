#include <Arduino.h>
#include <HardwareSerial.h>
#include <ESP8266WiFi.h>
#include "PubSubClient.h"

// Copy "details_template.h" and set the credentials
#include "details.h"

void reconnect();
void setup_wifi();
void handleInterrupt(uint8_t num);

static const uint8_t count = 2;
static const uint8_t intPins[] = {5, 4}; // D1, D2
static const char* topics[] = {MQTT_PLUG_TOPIC, MQTT_LIGHT_TOPIC};
static void (*funcs[])() = {[]{handleInterrupt(0);}, []{handleInterrupt(1);}};

volatile uint32_t interruptCounters[] = {0, 0};
volatile uint64_t lastHits[] = {0, 0};

WiFiClient espClient;
PubSubClient client(espClient);

void handleInterrupt(uint8_t num)
{
    uint64_t time = millis();
    if(time - lastHits[num] > 100) {
        interruptCounters[num]++;
    }
    lastHits[num] = time;
}

void setup()
{
    for(uint8_t i = 0; i < count; i++) {
        pinMode(intPins[0], INPUT_PULLUP);
        pinMode(intPins[1], INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(intPins[i]), funcs[i], RISING);
    }

    Serial.begin(115200);

    setup_wifi();
    client.setServer(MQTT_SERVER_IP, 1883);
}

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        // If you do not want to use a username and password, change next line to
        // if (client.connect("ESP8266Client")) {
        if (client.connect("ESP8266Client", MQTT_USER, MQTT_PASSWORD)) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 1 seconds");
            // Wait 1 second before retrying
            delay(1000);
        }
    }
}

void loop()
{
    if (!client.connected()) {
        Serial.println(client.state());
        reconnect();
    }
    client.loop();

    // Serial.println(interruptCounters[0]);
    // Serial.println(interruptCounters[1]);

    for(uint8_t i = 0; i < count; i++) {
        if(interruptCounters[i])
        {
            interruptCounters[i] = 0;
            Serial.print("Publish ");
            Serial.println(i);
            client.publish(topics[i], "TOGGLE", true);
        }
    }
    delay(1);
}
