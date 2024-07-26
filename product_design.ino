#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>  
#include <Adafruit_Sensor.h>
#include <DHT_U.h>

#define DHTTYPE    DHT11     // DHT 11
#define DHTPIN 2
#define DS18B20 5
#define REPORTING_PERIOD_MS     1000

float temperature, humidity, BPM, SpO2, bodytemperature;

/*Put your SSID & Password*/
const char* ssid = "May the Wifi be with you";  // Enter SSID here
const char* password = "THUNDERshows2045";  //Enter Password here

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
PulseOximeter pox;
uint32_t tsLastReport = 0;
OneWire oneWire(DS18B20);
DallasTemperature sensors(&oneWire);

WebServer server(80);

void onBeatDetected()
{
  Serial.println("Beat!");
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  sensor_t sensor;
  delayMS = sensor.min_delay / 1000;
  pinMode(19, OUTPUT);
  delay(100);   

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

  Serial.print("Initializing pulse oximeter..");

  //  if (!pox.begin()) {
  //    Serial.println("FAILED");
  //    for (;;);
  //  } else {
  //    Serial.println("SUCCESS");
  //    pox.setOnBeatDetectedCallback(onBeatDetected);
  //  }

  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  // Register a callback for the beat detection
}

void loop() {
  server.handleClient();
  pox.update();
  sensors.requestTemperatures();

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  } else {
    temperature = event.temperature;
  }
  delay(1000);
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  } else {
    humidity = event.relative_humidity;
  }
  delay(1000);
  BPM = 78;
  SpO2 = 5;
  //bodytemperature = 6;
  bodytemperature = sensors.getTempCByIndex(0);

  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    Serial.print("Room Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");

    Serial.print("Room Humidity: ");
    Serial.print(humidity);
    Serial.println("%");

    Serial.print("BPM: ");
    Serial.println(BPM);

    Serial.print("SpO2: ");
    Serial.println(SpO2);

    Serial.print("Body Temperature: ");
    Serial.print(bodytemperature);
    Serial.println("°C");

    tsLastReport = millis();
  }
}

void handle_OnConnect() {
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  } else {
    temperature = event.temperature;
  }
  delay(1000);
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  } else {
    humidity = event.relative_humidity;
  }
  delay(1000);
  BPM = 78;
  SpO2 = 5;
  //bodytemperature = 6;
  bodytemperature = sensors.getTempCByIndex(0);

  String s = SendHTML(temperature, humidity, BPM, SpO2, bodytemperature);
  server.send(200, "text/html", s);
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float temperature, float humidity, float BPM, float SpO2, float bodytemperature) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Health Monitoring System</title>\n";
  ptr += "<style>html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "h1 {color: #0F3376; padding: 2vh;}\n";
  ptr += "p {font-size: 1.5rem;}</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>Health Monitoring System</h1>\n";
  ptr += "<p>Room Temperature: " + String(temperature) + "°C</p>";
  ptr += "<p>Room Humidity: " + String(humidity) + "%</p>";
  ptr += "<p>BPM: " + String(BPM) + "</p>";
  ptr += "<p>SpO2: " + String(SpO2) + "%</p>";
  ptr += "<p>Body Temperature: " + String(bodytemperature) + "°C</p>";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
