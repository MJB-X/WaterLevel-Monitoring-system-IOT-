#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

#include <FS.h>
#include <ArduinoJson.h>

// Wi-Fi credentials
const char* ssid = "Your Wifi Name";
const char* password = "Your wifi password";



// Ultrasonic sensor pins
const int triggerPin = D1;
const int echoPin = D2;

 // Tank settings variables
 int tankHeight = 100;
 int maxWaterLevel = 80;

 // Alert settings variables
bool alertBelowEnabled = false;
bool alertAboveEnabled = false;
int belowLevelPercentage = 20; 
int aboveLevelPercentage = 80; 
int belowToneFrequency = 1000;  
int aboveToneFrequency = 2000; 

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");  

unsigned long previousMillis = 0;
long interval = 2000;  

// Function to calculate distance  
long getDistance() {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.0343 / 2;
  return distance;
}

// Function to calculate water level with Offset
int getWaterLevel() {
  int offset=(tankHeight - maxWaterLevel);
  long distance = getDistance();

  
  if (distance > tankHeight) {
    return 0;  
  } else if (distance <= offset) {
    return 100;  
  } else {
    
    int percentage = ((tankHeight - distance) * 100) / (tankHeight - offset);
    return percentage;
  }
}


// WebSocket event handler
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_DATA) {
    // Parse received data as JSON
    String receivedData = "";
    for (size_t i = 0; i < len; i++) {
      receivedData += (char) data[i];
    }

    // Convert the received data into JSON object
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, receivedData);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.f_str());
      return;
    }

    String requestType = doc["type"];

    // Handle request to get current settings
if (requestType == "getSettings") {
  DynamicJsonDocument response(1024);
  response["type"] = "settingsData";
  response["tankHeight"] = tankHeight;
  response["maxLevel"] = maxWaterLevel;
  response["frequency"] = interval / 1000;

  // Include alert settings in the response
  response["alertBelowEnabled"] = alertBelowEnabled;
  response["belowLevelPercentage"] = belowLevelPercentage;
  response["belowToneFrequency"] = belowToneFrequency;
  
  response["alertAboveEnabled"] = alertAboveEnabled;
  response["aboveLevelPercentage"] = aboveLevelPercentage;
  response["aboveToneFrequency"] = aboveToneFrequency;

  String responseData;
  serializeJson(response, responseData);
  client->text(responseData);
}
 else if (requestType == "updateSettings") {
      // Handle request to update settings
      int newTankHeight = doc["tankHeight"];
      int newMaxLevel = doc["maxLevel"];
      int newFrequency = doc["frequency"];

      // Update the settings
      tankHeight = newTankHeight;
      maxWaterLevel = newMaxLevel;
      interval = newFrequency * 1000;

      // Alert setting recived from client
      alertBelowEnabled = doc["enableAlert"];
      belowLevelPercentage = doc["belowLevelPercentage"];
      belowToneFrequency = doc["belowToneFrequency"];
  
    alertAboveEnabled = doc["enableAlert"];
    aboveLevelPercentage = doc["aboveLevelPercentage"];
    aboveToneFrequency = doc["aboveToneFrequency"];


      // Send a confirmation response back to the client
      DynamicJsonDocument response(512);
      response["type"] = "updateConfirmation";
      response["status"] = "Settings updated";

      String responseData;
      serializeJson(response, responseData);
      client->text(responseData);
    }
    else if(requestType == "requestWaterLevel") {
       Serial.println("Gettign water level request from client");
      int waterLevel = getWaterLevel();
      DynamicJsonDocument response(512);
      response["type"] = "waterLevel";
      response["level"] = waterLevel;
      String responseData;
      serializeJson(response, responseData);
      client->text(responseData);
      Serial.println("Sending water level to client: " + String(waterLevel));
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(triggerPin, OUTPUT); 
  pinMode(echoPin, INPUT);

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }


  // Connect to Wi-Fi
  WiFi.begin(ssid, password);

  Serial.println("Connecting to WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Serve the index.html file
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  
  // Serve the tank-settings.html file
  server.on("/tank-settings", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/tank-settings.html", "text/html");
  });

  // Initialize WebSocket and add an event handler
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  // Start the server
  server.begin();
  Serial.println("Server started");
}

void loop() {

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    int waterLevel = getWaterLevel();
    bool alertBelowTriggered = alertBelowEnabled && (waterLevel <= belowLevelPercentage);
    bool alertAboveTriggered = alertAboveEnabled && (waterLevel >= aboveLevelPercentage);

    DynamicJsonDocument message(256);
    message["level"] = waterLevel;

    // Add alert state and tone data
    if (alertBelowTriggered) {
      message["alert"] = "below";
      message["toneFrequency"] = belowToneFrequency;
    } else if (alertAboveTriggered) {
      message["alert"] = "above";
      message["toneFrequency"] = aboveToneFrequency;
    } else {
      message["alert"] = "none";
      message["toneFrequency"] = 0; 
    }

    String messageStr;
    serializeJson(message, messageStr);
    ws.textAll(messageStr);


    Serial.println("Broadcasting data: " + messageStr);
  }

  ws.cleanupClients();
}
