#include <Arduino.h>
#include "Adafruit_Thermal.h"

#include "photo.h"
#include "tutore.h"

#include <WiFi.h>
#include <WebServer.h>

// const char* ssid[] = { "SSID" };
// const char* password[] = { "password" };
// We have that in config.h
#include "config.h"

WebServer server(80);

Adafruit_Thermal printer(&Serial2, 4);

void html_index();
void html_index_print();
void html_ticket();
void html_photo();
void handle_NotFound();

void setup() {
  Serial.begin(9600);

  Serial.print("Connecting to ");
  Serial.println(ssid[0]);
  WiFi.begin(ssid[0], password[0]);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, html_index);
  server.on("/", HTTP_POST, html_index_print);
  server.on("/ticket", html_ticket);
  server.on("/photo", html_photo);
  server.onNotFound(handle_NotFound);
  server.begin();

  Serial2.begin(9600);
  printer.setDefault();
  printer.begin();
  printer.justify('C');
  // printer.println(WiFi.localIP());
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

void html_index() {
  server.send(200, "text/html", index_html_data); 
}

void html_index_print() {
  server.send(200, "text/plain", "OK"); 
}

void html_ticket() {
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

void html_photo() {
  printer.justify('C');
  printer.printBitmap(photo_width, photo_height, photo_data);
  server.send(200, "text/html", index_html_data); 
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}
