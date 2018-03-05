#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "WiFiManager.h" 
#include <WebSocketsClient.h>
#include <Hash.h>

#define BLINK_COUNT 5
#define TARGET_PIN 15

char host[] = "ininfluent.herokuapp.com";
int port = 80;
int brightness = 255;     // how bright the LED is
int fadeAmount = 35;      // how many points to fade the LED by
int counter = 0;
float pulse = 0.7;

WebSocketsClient webSocket;

/*extern String RID;
extern String Rname;
extern String Rcontent;
*/
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}


void turnOffWiFi() {
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(1);
}

void blink() {
  while (true) {
    analogWrite(TARGET_PIN, brightness);
    brightness = brightness - (fadeAmount * pulse);

    if (brightness <= 10 && counter == 0) {
      brightness = 255 ;

      counter = 1;
      fadeAmount = 5;
    }
    else if (brightness <= 10 && counter == 1) {
      delay(32 * 10);
      brightness = 255;
      counter = 0;
      fadeAmount = 35;
    }

    
    delay(2 * 10);
  }
}


void onRequest() {
  turnOffWiFi();
  Serial.println("Yes :) Lets blynk and turn off WiFi!");
  blink();
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      break;
    case WStype_CONNECTED:
      break;
    case WStype_TEXT:
      String message = (char*)payload;
      if (message == "request") {
        onRequest();
      }
      break;
  }
}

void setup() { 
  Serial.begin(115200);
  Serial.setDebugOutput(false);

  pinMode(TARGET_PIN, OUTPUT);
  digitalWrite(TARGET_PIN, false);
  
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

   //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  Serial.println("provo a connettermi al webserver....");

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.begin(host, port, "/ws");
  webSocket.setReconnectInterval(500);
}



void loop() {
  webSocket.loop();
}