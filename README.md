# Electronic product-Design
# Vital Life Monitor

The Vital Life Monitor is a comprehensive health monitoring device designed to measure various vital signs such as heart rate, blood oxygen level (SpO2), temperature, and humidity. This project utilizes an Arduino microcontroller and various sensors to provide real-time health data.

## Features

- **Heart Rate Monitoring**: Measures BPM (Beats Per Minute).
- **Blood Oxygen Level**: Measures SpO2 (Blood Oxygen Saturation).
- **Temperature Measurement**: Records both ambient and body temperature.
- **Humidity Measurement**: Records the ambient humidity levels.
- **WiFi Connectivity**: Sends data to a web server for remote monitoring.

## Components Used

- Arduino Board
- MAX30100 Pulse Oximeter
- DHT11 Temperature and Humidity Sensor
- DS18B20 Temperature Sensor
- WiFi Module
- Various resistors, wires, and connectors

## Installation

1. **Clone the repository**:
    ```sh
    git clone https://github.com/yourusername/vital-life-monitor.git
    ```
2. **Open the Arduino IDE** and load the `product_design.ino` file.
3. **Install the required libraries**:
    - WiFi
    - WebServer
    - Wire
    - MAX30100_PulseOximeter
    - OneWire
    - DallasTemperature
    - DHT
    - Adafruit_Sensor

4. **Upload the code to the Arduino**.

## Usage

1. **Connect the sensors to the Arduino** as per the circuit diagram.
2. **Power on the device** and ensure it connects to your WiFi network.
3. **Open a web browser** and navigate to the device's IP address to view the real-time data.

## Code Overview

The code initializes the sensors and WiFi module, reads data from the sensors, and serves the data over a web server.

```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

// Sensor and WiFi configuration
#define DHTTYPE DHT11
#define DHTPIN 2
#define DS18B20 5
#define REPORTING_PERIOD_MS 1000

// Replace with your network credentials
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

DHT_Unified dht(DHTPIN, DHTTYPE);
PulseOximeter pox;
OneWire oneWire(DS18B20);
DallasTemperature sensors(&oneWire);
WebServer server(80);

void setup() {
  Serial.begin(115200);
  dht.begin();
  sensors.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  pox.update();
  server.handleClient();
}

void handleRoot() {
  float temperature = sensors.getTempCByIndex(0);
  float humidity = dht.readHumidity();
  float BPM = pox.getHeartRate();
  float SpO2 = pox.getSpO2();
  
  String response = "Temperature: " + String(temperature) + " C\n";
  response += "Humidity: " + String(humidity) + " %\n";
  response += "Heart Rate: " + String(BPM) + " BPM\n";
  response += "SpO2: " + String(SpO2) + " %\n";
  
  server.send(200, "text/plain", response);
}
