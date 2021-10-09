#include "WiFiReconnect.h"

// We have that in include/config.h which is gitignored
// const uint8_t maxWiFiCount = 2;
// const char* WIFI_SSIDS[] = { "SSID", "SSID2" };
// const char* WIFI_PASSWORDS[] = { "password", "password2" };
#include "wifi_config.h"

uint8_t currentWiFi = 0;

void WiFiStationConnect() {
  Serial.print(F("Connecting to "));
  Serial.println(WIFI_SSIDS[currentWiFi]);
  WiFi.begin(WIFI_SSIDS[currentWiFi], WIFI_PASSWORDS[currentWiFi]);
}

void WiFiStationGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  digitalWrite(OUTPUT_LED_WIFI_CONNECTED, HIGH);
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
  configTzTime("CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", "time.google.com", "time.nist.gov", "pool.ntp.org");
  while (time(nullptr) < 1000000) {
    delay(500);
  }
  time_t now = time(nullptr);
  Serial.println(ctime(&now));
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
