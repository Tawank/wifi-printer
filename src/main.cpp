#include <Arduino.h>
#include "Adafruit_Thermal.h"

#include "photo.h"
#include "tutore.h"

#include <WiFi.h>
#include <WebServer.h>

// We have that in include/config.h which is gitignored
// const char* ssid[] = { "SSID" };
// const char* password[] = { "password" };
#include "config.h"

#define BOARD_LED 2

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

  Serial2.begin(9600);
  printer.setDefault();
  printer.begin();
  printer.justify('C');

  pinMode(BOARD_LED, OUTPUT);
}

void loop() {
  server.handleClient();
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
  // /ticket?title=Problem%20jakis%20tam&number=B214&clientCount=512
  printer.setSize('S');
  printer.justify('C');
  printer.printBitmap(tutore_width, tutore_height, tutore_data);
  printer.println(F(""));
  printer.println(server.arg("title"));
  printer.boldOn();
  printer.setSize('L');
  printer.println(server.arg("number"));
  printer.boldOff();
  printer.println(F(""));
  printer.setSize('S');
  printer.println("Petenci przed: " + server.arg("clientCount"));
  printer.println(F(""));
  printer.println(F(""));
  printer.println(F("Dziekujemy za wizyte!"));
  printer.println(F(""));
  printer.println(F(""));
  printer.println(F(""));
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
