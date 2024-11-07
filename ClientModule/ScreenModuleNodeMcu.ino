#include <ArduinoJson.h>



#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Hash.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiClient.h> 
#include <ESP8266HTTPClient.h>  // To make HTTP requests
#include <TimeLib.h>  // Time library to fetch current time

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

#define USE_SERIAL Serial
#define BUZZER_PIN D5
#define BUTTON_PIN D3
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "Your Wifi Name";
const char* password = "Your wifi password"; 

unsigned long lastWeatherUpdate = 0;
const unsigned long weatherUpdateInterval = 600000;  // 10 minutes

void updateDisplay(const char* wifiStatus, int waterLevel, const char* weekdayName) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Display weekday name
  display.setCursor(0, 17);
  display.printf("Today: %s", weekdayName);

  // Display WiFi status
  display.setCursor(0, 0);
  display.printf("WiFi: %s", wifiStatus);

  // Display water level text
  display.setCursor(0, 40);
  display.print("Water Level:");
  display.setCursor(0, 50);
  display.printf("%d %%", waterLevel);

  // Draw tank outline on the right side
  int tankX = 100;
  int tankY = 20;
  int tankWidth = 20;
  int tankHeight = 40;
  display.drawRect(tankX, tankY, tankWidth, tankHeight, SSD1306_WHITE);

  // Calculate water level height based on percentage
  int filledHeight = (waterLevel * tankHeight) / 100;
  int fillY = tankY + tankHeight - filledHeight;

  // Draw water level inside the tank
  display.fillRect(tankX + 1, fillY, tankWidth - 2, filledHeight, SSD1306_WHITE);

  display.display();
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      USE_SERIAL.printf("[WSc] Disconnected!\n");
      updateDisplay("Disconnected", -1, getWeekdayName(weekday(now())));
      break;
    case WStype_CONNECTED:
      USE_SERIAL.printf("[WSc] Connected to URL: %s\n", payload);
      updateDisplay("Connected", -1, getWeekdayName(weekday(now())));
      webSocket.sendTXT("Connected");
      break;
    case WStype_TEXT: {
      USE_SERIAL.printf("[WSc] Received text: %s\n", payload);

      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        int level = doc["level"] | -1;
        const char* alert = doc["alert"] | "";
        int toneFrequency = doc["toneFrequency"] | 0;

        // Update display with water level and weekday if received
        updateDisplay("Connected", level, getWeekdayName(weekday(now())));

        if (strcmp(alert, "above") == 0 || strcmp(alert, "below") == 0) {
          if (toneFrequency > 0) {
            tone(BUZZER_PIN, toneFrequency);
            delay(500);
            noTone(BUZZER_PIN);
          }
        } else {
          noTone(BUZZER_PIN);
        }
      } else {
        USE_SERIAL.println("Failed to parse JSON!");
      }
      break;
    }
    case WStype_BIN:
      USE_SERIAL.printf("[WSc] Received binary data of length: %u\n", length);
      hexdump(payload, length);
      break;
    case WStype_PING:
      USE_SERIAL.printf("[WSc] Received ping\n");
      break;
    case WStype_PONG:
      USE_SERIAL.printf("[WSc] Received pong\n");
      break;
  }
}
void requestWaterLevel() {
  StaticJsonDocument<512> request;
  request["type"] = "requestWaterLevel";

  String requestData;
  serializeJson(request, requestData);
  webSocket.sendTXT(requestData);
  USE_SERIAL.println("Requesting water level...");
}

const char* getWeekdayName(int weekdayNum) {
  const char* weekdayNames[] = {"Invalid", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  return weekdayNames[weekdayNum];
}
void setup() {
  USE_SERIAL.begin(115200);
  USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println("[SETUP] Starting...");

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(BUTTON_PIN, INPUT_PULLUP);


  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    USE_SERIAL.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.display();

  WiFiMulti.addAP(ssid, password);
  while(WiFiMulti.run() != WL_CONNECTED) {
    USE_SERIAL.print(".");
    delay(500);
  }
  USE_SERIAL.println("\n[SETUP] WiFi connected.");
 

  webSocket.begin("your_websocket_url", 80, "/ws");
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("user", "Password");
  webSocket.setReconnectInterval(5000);
  webSocket.enableHeartbeat(15000, 3000, 2);

  USE_SERIAL.println("[SETUP] WebSocket setup complete.");
}

void loop() {
  webSocket.loop();

  if (digitalRead(BUTTON_PIN) == LOW) {
    USE_SERIAL.println("Button pressed!");
    requestWaterLevel();
    delay(500);
  }

  
  int currentWeekday = weekday(now());
  const char* weekdayName = getWeekdayName(currentWeekday);

  
 

  delay(500);  
}
