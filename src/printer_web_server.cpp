#include "printer_web_server.h"
#include <Adafruit_Thermal.h>

#include "photo.h"
#include "tutore.h"

extern Adafruit_Thermal printer;
extern WebServer server;

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
      }
      continue;
    }
    printer.print(server.arg("plain")[i]);
  }

  Serial.println(server.arg("plain"));
  server.send(200, "text/plain", "OK");
}

void urlHandleIndexImagePrint() {
  // for (unsigned int i = 0; i < server.arg("plain").length(); i++) {
  //   if (server.arg("plain")[i] == 1) {
  //     server.arg("plain").setCharAt(i, 0);
  //     Serial.println(server.arg("plain")[i], DEC);
  //   }
  // }

  // printer.printBitmap(server.arg("width").toInt(), server.arg("height").toInt(), reinterpret_cast<const uint8_t*>(server.arg("plain").c_str()));

  Serial.println(server.uri());
  server.send(200, "text/plain", "OK");
}

void urlHandleFileUploadPrint() {
  Serial.println(server.uri());
  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
    Serial.print("Upload: START, filename: ");
    Serial.println(upload.filename);
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    printer.printBitmap(server.arg("width").toInt(), server.arg("height").toInt(), upload.buf);
    Serial.print("Upload: WRITE, Bytes: ");
    Serial.println(upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    Serial.print("Upload: END, Size: ");
    Serial.println(upload.totalSize);
  }
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
  server.send(200, "text/plain", "OK");
}

void urlHandlePhoto() {
  printer.justify('C');
  printer.printBitmap(photo_width, photo_height, photo_data, true);
  server.send(200, "text/plain", "OK");
}

void urlHandleNotFound() {
  server.send(404, "text/plain", "Not found");
}
