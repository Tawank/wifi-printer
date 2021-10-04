#ifndef _wifi_reconnect_h_
#define _wifi_reconnect_h_
#include <WiFi.h>

#define OUTPUT_LED_WIFI_CONNECTED 2

void WiFiStationConnect();
void WiFiStationGotIP(WiFiEvent_t, WiFiEventInfo_t);
void WiFiStationLostIP(WiFiEvent_t, WiFiEventInfo_t);
void WiFiStationDisconnected(WiFiEvent_t, WiFiEventInfo_t);

#endif // _wifi_reconnect_h_
