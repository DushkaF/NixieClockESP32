#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>

String ssid = "";
String password = "";

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
    }
  }
  Serial.println("");

  String JsonLine = "";
  serializeJson(APlist, JsonLine);
  return JsonLine;
}

void connectToWifi() {
  String status = "";
  WiFi.begin(ssid.c_str(), password.c_str());
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    status = "501";
  } else{
    status = String(WiFi.localIP());
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
  //return status;
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

      server.on("/selectedAp", HTTP_POST, [](AsyncWebServerRequest * request) {
        if (request->params() > 0) {
          AsyncWebParameter* p = request->getParam(0);
          String json = p->value();
          Serial.println(json);
          StaticJsonDocument<64> doc;
          deserializeJson(doc, json);
          ssid = doc["name"].as<String>();
          password = doc["password"].as<String>();
          Serial.println(ssid + " " + password);
        }
        request->send(200);
        //connectToWifi();
      });
    }
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest * request) {
      //request->addInterestingHeader("ANY");
      return true;
    }

    void handleRequest(AsyncWebServerRequest * request) {
      request->send(SPIFFS, "/inner_page/html/index.html", String(), false);
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Start");
  WiFi.mode(WIFI_AP_STA);

  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.softAP("NixieClock");
  dnsServer.start(53, "*", WiFi.softAPIP());

  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
  if(ssid != ""){
    connectToWifi();
  }
}
