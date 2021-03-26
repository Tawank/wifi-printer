#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_Thermal.h>

#include "photo2.h"
#include "tutore.h"

// We have that in include/config.h which is gitignored
// const uint8_t maxWiFiCount = 2;
// const char* ssid[] = { "SSID", "SSID2" };
// const char* password[] = { "password", "password2" };
#include "config.h"

#define BOARD_LED 2
#define PRINT_IP_BUTTON 15

WebServer server(80);

Adafruit_Thermal printer(&Serial2, 4);

void urlHandleIndex();
void urlHandleIndexPrint();
void urlHandleTicket();
void urlHandlePhoto();
void urlHandleNotFound();

void WiFiStationConnect();
void WiFiStationGotIP(WiFiEvent_t, WiFiEventInfo_t);
void WiFiStationLostIP(WiFiEvent_t, WiFiEventInfo_t);
void WiFiStationDisconnected(WiFiEvent_t, WiFiEventInfo_t);

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
  server.on("/ticket", urlHandleTicket);
  server.on("/photo", urlHandlePhoto);
  server.onNotFound(urlHandleNotFound);
  server.begin();

  printer.setDefault();
  printer.begin(200);

  pinMode(BOARD_LED, OUTPUT);
  pinMode(5, INPUT_PULLUP);
}

void loop() {
  server.handleClient();

  if (digitalRead(PRINT_IP_BUTTON) == 0) {
    printer.justify('C');
    printer.println(WiFi.localIP());
    printer.feed();
    printer.feed();
    delay(700);
  }
}

const char *index_html_data =
  "<!DOCTYPE html><html>"
  "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
  "<link rel=\"icon\" href=\"data:,\">"
  "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"
  ".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;"
  "text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}"
  ".button2 {background-color: #555555;}</style></head>"
  "<body><h1>OK</h1>"
  "</body></html>";

void urlHandleIndex() {
  server.send(200, "text/html", index_html_data); 
}

void urlHandleIndexPrint() {
  server.send(200, "text/plain", "OK"); 
}

void urlHandleTicket() {
  printer.setSize('S');
  printer.justify('C');
  printer.printBitmap(tutore_width, tutore_height, tutore_data);
  printer.feed();
  printer.println(server.arg("title"));
  printer.boldOn();
  printer.setSize('L');
  printer.println(server.arg("number"));
  printer.boldOff();
  printer.feed();
  printer.setSize('S');
  printer.print(F("Petenci przed: "));
  printer.println(server.arg("clientCount"));
  printer.feed();
  printer.feed();
  printer.println(F("Dziekujemy za wizyte!"));
  printer.feed();
  printer.feed();
  printer.feed();
  server.send(200, "text/html", index_html_data); 
}

void urlHandlePhoto() {
  printer.justify('C');
  printer.printBitmap(photo_width, photo_height, photo_data, true);
  server.send(200, "text/html", index_html_data); 
}

void urlHandleNotFound() {
  server.send(404, "text/plain", "Not found");
}

uint8_t currentWiFi = 0;

void WiFiStationConnect() {
  Serial.print(F("Connecting to "));
  Serial.println(ssid[currentWiFi]);
  WiFi.begin(ssid[currentWiFi], password[currentWiFi]);
}

void WiFiStationGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
  digitalWrite(BOARD_LED, HIGH);
}

void WiFiStationLostIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.print(F("IP lost"));
  digitalWrite(BOARD_LED, LOW);
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.print(F("WiFi lost connection. Reason: "));
  Serial.println(info.disconnected.reason);
  if (info.disconnected.reason == WIFI_REASON_NO_AP_FOUND) {
    currentWiFi = (currentWiFi + 1) % maxWiFiCount;
  }
  WiFi.disconnect();
  WiFiStationConnect();
  digitalWrite(BOARD_LED, LOW);
}
