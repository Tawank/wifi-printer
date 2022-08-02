#include "MyWebServer.h"
#include <Base64.hpp>

#include "photo.h"
#include "tutore.h"

extern Adafruit_Thermal printer;
extern WebServer server;

const char PROGMEM *index_html_data =
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
  for (unsigned int i = 0; i < server.arg("plain").length(); i++) {
    if (server.arg("plain")[i] == '$') {
      i++;
      switch (server.arg("plain")[i]) {
        case '$':
          printer.print('$');
          break;
        case 'B':
          printer.boldOn();
          break;
        case 'b':
          printer.boldOff();
          break;
        case 'S':
          printer.setSize('S');
          break;
        case 'M':
          printer.setSize('M');
          break;
        case 'L':
          printer.setSize('L');
          break;
        case 'W':
          printer.doubleWidthOn();
          break;
        case 'w':
          printer.doubleWidthOff();
          break;
        case 'H':
          printer.doubleHeightOn();
          break;
        case 'h':
          printer.doubleHeightOff();
          break;
        case 'l':
          printer.justify('l');
          break;
        case 'c':
          printer.justify('c');
          break;
        case 'r':
          printer.justify('r');
          break;
      }
      continue;
    }
    printer.print(server.arg("plain")[i]);
  }
  printer.feed();

  Serial.println(server.arg("plain"));
  server.send(200, "text/plain", "OK");
}

void urlHandleIndexImagePrint() {
  if ((server.arg("width").toInt() + server.arg("height").toInt()) > 1000) {
    return server.send(400, F("text/plain"), "Image is too big");
  }

  unsigned char* buffer = (unsigned char*) malloc (server.arg("width").toInt() * server.arg("height").toInt() / 8 + 1);
  if (buffer == NULL) {
    return server.send(500, F("text/plain"), "Could not allocate buffer");
  }
  decode_base64((unsigned char*) server.arg("plain").c_str(), buffer);
  printer.printBitmap(server.arg("width").toInt(), server.arg("height").toInt(), buffer);
  printer.feed(2);

  free(buffer);
  server.send(200, F("text/plain"), "OK");
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
  printer.feed(2);
  printer.println(F("Dziekujemy za wizyte!"));
  printer.feed(3);
  server.send(200, F("text/plain"), "OK");
}

void urlHandlePhoto() {
  printer.justify('C');
  printer.printBitmap(photo_width, photo_height, photo_data, true);
  server.send(200, F("text/plain"), "OK");
}

void urlHandleNotFound() {
  server.send(404, F("text/plain"), "Not found");
}
