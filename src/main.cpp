#include <Arduino.h>
#include <Adafruit_Thermal.h>

#include <wifi_reconnect.h>
#include "printer_web_server.h"

#define INPUT_BUTTON_PRINT_IP 15

WebServer server(80);

Adafruit_Thermal printer(&Serial2, 4);

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);

  WiFi.setAutoReconnect(false);
  WiFi.onEvent(WiFiStationGotIP, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(WiFiStationLostIP, SYSTEM_EVENT_STA_LOST_IP);
  WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
  WiFiStationConnect();

  server.on("/", HTTP_GET, urlHandleIndex);
  server.on("/", HTTP_POST, urlHandleIndexPrint);
  server.on("/image", HTTP_POST, urlHandleIndexImagePrint, urlHandleFileUploadPrint);
  server.on("/ticket", urlHandleTicket);
  server.on("/photo", urlHandlePhoto);
  server.onNotFound(urlHandleNotFound);
  server.begin();

  printer.setDefault();
  printer.begin();

  pinMode(OUTPUT_LED_WIFI_CONNECTED, OUTPUT);
  pinMode(5, INPUT_PULLUP);
}

void loop() {
  server.handleClient();

  if (digitalRead(INPUT_BUTTON_PRINT_IP) == 0) {
    printer.justify('C');
    printer.println(WiFi.localIP());
    printer.feed(2);
    printer.justify('L');
    delay(700);
  }
}
