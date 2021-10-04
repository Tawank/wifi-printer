#include <Arduino.h>
#include "WiFiReconnect.h"

// We have that in include/config.h which is gitignored
// const uint8_t maxWiFiCount = 2;
// const char* ssid[] = { "SSID", "SSID2" };
// const char* password[] = { "password", "password2" };
#include "wifi_config.h"


uint8_t currentWiFi = 0;

void WiFiStationConnect() {
  Serial.print(F("Connecting to "));
  Serial.println(ssid[currentWiFi]);
  WiFi.begin(ssid[currentWiFi], password[currentWiFi]);
}

void WiFiStationGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
  digitalWrite(OUTPUT_LED_WIFI_CONNECTED, HIGH);
}

void WiFiStationLostIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.print(F("IP lost"));
  digitalWrite(OUTPUT_LED_WIFI_CONNECTED, LOW);
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.print(F("WiFi lost connection. Reason: "));
  Serial.println(info.disconnected.reason);
  if (info.disconnected.reason == WIFI_REASON_NO_AP_FOUND) {
    currentWiFi = (currentWiFi + 1) % maxWiFiCount;
  }
  WiFi.disconnect();
  WiFiStationConnect();
  digitalWrite(OUTPUT_LED_WIFI_CONNECTED, LOW);
}
