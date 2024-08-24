#include <Wire.h>
#include "SHT3xSensor.h"

#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

// for sensor read data 
SHT3xSensor sht3x;
float temperature = 0.0;
float humidity = 0.0;

// for send data via esp_Now
    // Set your Board ID (ESP32 Sender #1 = BOARD_ID 1, ESP32 Sender #2 = BOARD_ID 2, etc)
#define BOARD_ID F2_S01
//MAC Address of the receiver 
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

    if (!sht3x.begin()) {
        Serial.println("Failed to initialize SHT3x sensor.");
        while (true); // Stop execution if initialization fails
    }
}

void loop() {
    // float temperature = 0.0;
    // float humidity = 0.0;

    if (sht3x.readMeasurement(temperature, humidity)) {
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print(" C\tHumidity: ");
        Serial.print(humidity);
        Serial.println(" %");
    } else {
        Serial.print("Error reading measurement: ");
        Serial.println(sht3x.getLastError());
    }

    delay(1000); // Adjust the delay as needed


}
