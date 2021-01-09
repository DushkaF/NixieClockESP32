#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>

DNSServer dnsServer;
AsyncWebServer server(80);

class CaptiveRequestHandler : public AsyncWebHandler {
  public:
    CaptiveRequestHandler() {
      /* THIS IS WHERE YOU CAN PLACE THE CALLS */

      server.on("/style/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/inner_page/style/style.css", "text/css");
         request->send(response);
      });
    }
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request) {
      //request->addInterestingHeader("ANY");
      return true;
    }

    void handleRequest(AsyncWebServerRequest *request) {
      request->send(SPIFFS, "/inner_page/html/index.html", String(), false);
    }
};

void setup() {

  Serial.begin(115200);

  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.softAP("NixieClock");
  dnsServer.start(53, "*", WiFi.softAPIP());

  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP

  /*server.on("/image1", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/image1.jpg", "image/jpg"); // this part has been modified
  });*/

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
}
