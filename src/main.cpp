#include <Wire.h>
#include "SHT3xSensor.h"

#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

SHT3xSensor sht3x;

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
    float temperature = 0.0;
    float humidity = 0.0;

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
