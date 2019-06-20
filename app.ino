#include <Arduino.h>
#include <HardwareSerial.h>
#include <ESP8266WiFi.h>
#include "PubSubClient.h"

// Copy "details_template.h" and set the credentials
#include "details.h"

void reconnect();
void setup_wifi();

static const uint8_t greenPin = A0;
static const uint8_t intPin = 5;
// static const uint8_t redPin = A1;
// static const uint8_t bluePin = A2;
uint16_t greenForce;
uint16_t redForce;
// uint16_t blueForce;
uint64_t lastHit;

volatile uint32_t interruptCounter = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void handleInterrupt()
{
    uint64_t time = millis();
    if(time - lastHit > 100) {
        interruptCounter++;
    }
    lastHit = time;
}

void setup()
{
    // p.Begin(); // start plotter

    // pinMode(greenPin, INPUT);
    pinMode(intPin, INPUT_PULLUP);
    // pinMode(redPin, INPUT);
    // pinMode(bluePin, INPUT);

    Serial.begin(115200);

    // setup_wifi();
    // client.setServer(mqtt_server, 1883);

    lastHit = 0;

    attachInterrupt(digitalPinToInterrupt(intPin), handleInterrupt, FALLING);
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
    // if (!client.connected()) {
    //     Serial.println(client.state());
    //     reconnect();
    // }
    // client.loop();

    // greenForce = analogRead(greenPin);
    // redForce = analogRead(redPin);
    // blueForce = analogRead(bluePin);
    // Serial.println(interruptCounter);
    Serial.println(interruptCounter);

    // if(interruptCounter)
    // {
    //     Serial.println("Publish");
    //     client.publish(mqtt_topic, "TOGGLE", true);
    // }
    delay(1);
}
