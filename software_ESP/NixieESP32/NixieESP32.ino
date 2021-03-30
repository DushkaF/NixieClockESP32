#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>

String ssid = "";
String password = "";

unsigned long last_time = 0;

//--- for route HTTP ----
StaticJsonDocument<1024> networks;
boolean networksScanning = false;

boolean connectingToNetwork = false;
String connectionStatus = "";

boolean softAPenable = true;
boolean WiFienabled = false;

DNSServer dnsServer;
AsyncWebServer server(80);

StaticJsonDocument<1024> getNetworksJSON() {
  StaticJsonDocument<1024> APlist;
  int status = 0;
  APlist["status"] = status;
  JsonArray names = APlist.createNestedArray("name");
  Serial.println("scan start");
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
    status = 204;
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      names.add(WiFi.SSID(i));
    }
    status = 200;
  }
  APlist["status"] = status;
  return APlist;
}

String connectToWifi() {
  String status = "";
  WiFi.begin(ssid.c_str(), password.c_str());
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    status = "501";
  } else {
    status = WiFi.localIP().toString().c_str();
    //Serial.print("IP Address: ");
    //Serial.println(WiFi.localIP());
  }
  return status;
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
        int requestStatus = 202;
        String requestJSON = "";
        if (!networksScanning) {
          networksScanning = true;
        } else {
          if (!networks.isNull()) {
            requestStatus = networks["status"].as<int>();
            networks.remove("status");
            serializeJson(networks, requestJSON);
            networksScanning = false;
            Serial.println("end scanning");
            networks.clear();
          }
        }
        //AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/inner_page/APlist.json", "application/json");
        AsyncWebServerResponse* response = request->beginResponse(requestStatus, "application/json", requestJSON);
        request->send(response);
      });

      server.on("/selectedAp", HTTP_POST, [](AsyncWebServerRequest * request) {
        if (!connectingToNetwork) {
          if (request->params() > 0) {
            AsyncWebParameter* p = request->getParam(0);
            String json = p->value();
            Serial.println(json);
            StaticJsonDocument<256> doc;
            deserializeJson(doc, json);
            ssid = doc["name"].as<String>();
            password = doc["password"].as<String>();
            Serial.println(ssid + " " + password);
            connectingToNetwork = true;
            connectionStatus = "";
          }
        }
        request->send(202);
      });


      server.on("/selectedAp", HTTP_GET, [](AsyncWebServerRequest * request) {
        int requestStatus = 202;
        String requestJSON = "";
        if (connectingToNetwork) {
          if (connectionStatus != "") {
            connectingToNetwork = false;
            if (connectionStatus == "501") {
              requestStatus = 501;
            } else {
              //Serial.println(connectionStatus);
              Serial.println("Connected");
              StaticJsonDocument<64> doc;
              doc["ip"] = connectionStatus;
              serializeJson(doc, requestJSON);
              requestStatus = 200;
              Serial.println(requestJSON);
            }
          }
        }
        AsyncWebServerResponse* response = request->beginResponse(requestStatus, "application/json", requestJSON);
        request->send(response);
      });

      server.on("/redirected", HTTP_GET, [](AsyncWebServerRequest * request) {
        Serial.println("redirected");
        softAPenable = false;
        request->send(200);
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

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
</head>
<body>
  <h2>ESP Web Server</h2>
</body>
</html>
)rawliteral";


void setRoutesForNetwork(){
  server.on("/style/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
        AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/home_page/style/style.css", "text/css");
        request->send(response);
      });

      server.on("/js/script.js", HTTP_GET, [](AsyncWebServerRequest * request) {
        AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/home_page/js/script.js");
        request->send(response);
      });
  
      server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(SPIFFS, "/home_page/html/index.html", String(), false);
      });
}


void setup() {
  Serial.begin(115200);
  Serial.println("Start");
  WiFi.mode(WIFI_AP_STA);

  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  /*
    networks = getNetworksJSON();
    serializeJson(networks, Serial);
    Serial.println();
    networks.remove("status");
    serializeJson(networks, Serial);
    Serial.println();
    Serial.println(networks["status"].as<int>());*/

  if (softAPenable) {
    WiFi.softAP("NixieClock");
    dnsServer.start(53, "*", WiFi.softAPIP());
    server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP
    server.begin();
  }
}

void loop() {

  if (softAPenable) {
    dnsServer.processNextRequest();
    WiFienabled = false;
  } else {
    if (!WiFienabled) {
      WiFienabled = true;
      WiFi.softAPdisconnect(true);
      Serial.println("AP turn off"); 
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      dnsServer.stop();
      server.reset();
//      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
//        request->send_P(200, "text/html", index_html);
//      });
      setRoutesForNetwork();
      server.begin();
    }
  }

  if (connectingToNetwork && connectionStatus == "") {
    connectionStatus = connectToWifi();
  }

  if (networksScanning && networks.isNull()) {
    networks = getNetworksJSON();
  }
}
