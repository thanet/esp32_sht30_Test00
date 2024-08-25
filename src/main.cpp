#include <Wire.h>
#include "SHT3xSensor.h"

#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

// for sensor read data  Globle var
SHT3xSensor sht3x;
float temperature = 0.0;
float humidity = 0.0;
// -- for sensor read data Globle var

// for send data via esp_Now
// Set your Board ID (ESP32 Sender #1 = BOARD_ID 1, ESP32 Sender #2 = BOARD_ID 2, etc)
#define BOARD_ID 21
//MAC Address of the receiver 
uint8_t broadcastAddress[] = {0xCC, 0xDB, 0xA7, 0x31, 0x1A, 0x7C};
//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    int id;
    float temp;
    float hum;
    int readingId;
} struct_message;

esp_now_peer_info_t peerInfo;

//Create a struct_message called myData
struct_message myData;

unsigned long previousMillis = 0;   // Stores last time temperature was published
const long interval = 10000;        // Interval at which to publish sensor readings

unsigned int readingId = 0;

// Insert your SSID
constexpr char WIFI_SSID[] = "Esp_Now";

int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
      for (uint8_t i=0; i<n; i++) {
          if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
              return WiFi.channel(i);
          }
      }
  }
  return 0;
}

// ++callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
// --callback when data is sent

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

    if (!sht3x.begin()) {
        Serial.println("Failed to initialize SHT3x sensor.");
        while (true); // Stop execution if initialization fails
    }

// Set device as a Wi-Fi Station and set channel
    WiFi.mode(WIFI_STA);

    int32_t channel = getWiFiChannel(WIFI_SSID);

    WiFi.printDiag(Serial); // Uncomment to verify channel number before
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    esp_wifi_set_promiscuous(false);
    WiFi.printDiag(Serial); // Uncomment to verify channel change after

// Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

// Once ESPNow is successfully Init, we will register for Send CB to
// get the status of Trasnmitted packet
    esp_now_register_send_cb(OnDataSent);
    
    // Register peer
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.encrypt = false;
    
    // Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
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


// ++send data via Esp_Now
     unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
    // Save the last time a new reading was published
    previousMillis = currentMillis;
    //Set values to send
    myData.id = BOARD_ID;
    myData.temp = temperature;
    myData.hum = humidity;
    myData.readingId = readingId++;
     
    //Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
  } 

// --send data via Esp_Now
    delay(2000); // Adjust the delay as needed


}
