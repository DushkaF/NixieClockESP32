#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>



unsigned long last_time = 0;

DNSServer dnsServer;
AsyncWebServer server(80);

String getNetworksJSON() {
  StaticJsonDocument<256> APlist;
  JsonArray names = APlist.createNestedArray("name");
  
  Serial.println("scan start");
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      names.add(WiFi.SSID(i));
      // Print SSID and RSSI for each network found
      /*Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      delay(10);*/
    }
  }
  Serial.println("");

  String JsonLine = "";
  serializeJson(APlist, JsonLine);
  return JsonLine;
}


class CaptiveRequestHandler : public AsyncWebHandler {
  public:
    CaptiveRequestHandler() {
      /* THIS IS WHERE YOU CAN PLACE THE CALLS */

      server.on("/style/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
        AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/inner_page/style/style.css", "text/css");
        request->send(response);
      });

      server.on("/js/script.js", HTTP_GET, [](AsyncWebServerRequest * request) {
        AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/inner_page/js/script.js");
        request->send(response);
      });

      server.on("/APlist.json", HTTP_GET, [](AsyncWebServerRequest * request) {
        //AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/inner_page/APlist.json", "application/json");
        AsyncWebServerResponse* response = request->beginResponse(200, "application/json", getNetworksJSON());
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
  WiFi.mode(WIFI_AP_STA);
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

  /*if (millis() - last_time >= 5000) {
    Serial.println(getNetworksJSON());
    last_time = millis();
  }*/
}
