#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
//#include "index.h"
#include <FS.h>


// Web интерфейс для устройства
ESP8266WebServer server(80);
// Для файловой системы
File fsUploadFile;

String XML, data;


#define STASSID "SkyNet99"    //--------------- Подлежит удалению ----------//
#define STAPSK  "89119627235"
const char *ssid = STASSID;
const char *pass = STAPSK;


void handleForm() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    String message = "POST form was:\n";
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    //Serial.println(message);
    if (server.arg("a").equals("on"))
      digitalWrite(LED_BUILTIN, HIGH);
    else
      digitalWrite(LED_BUILTIN, LOW);
  }
}

void XMLcontent() {
  data = (String)(millis() / 1000);
  XML = "<?xml version='1.0'?>";
  XML += "<data>";
  XML += data;
  XML += "</data>";
  //Serial.println(data);
  server.send(200, "text/xml", XML);
}

void setup() {
  delay(1000);
  Serial.begin(115200);

  if (!SPIFFS.begin())
  {
    // Serious problem
    Serial.println("SPIFFS Mount failed");
  } else {
    Serial.println("SPIFFS Mount succesfull");
  }

  Serial.println("Creating AP WiFi..");

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  /* //connect to your local wi-fi network
    WiFi.mode(WIFI_AP);      // Режим точки доступа
    WiFi.softAP("ESP8266");

    //check wi-fi is connected to wi-fi network
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);*/


  server.on("/data.xml", XMLcontent);
  server.on("/postform/", handleForm);
  server.begin();
  server.serveStatic("/script.js", SPIFFS, "/script.js");
  server.serveStatic("/style.css", SPIFFS, "/style.css");
  server.serveStatic("/", SPIFFS, "/index.html");

  Serial.println("HTTP server started");
  
  pinMode(LED_BUILTIN, OUTPUT);     // Выбери 1 пин!
}

void loop() {
  server.handleClient();
}
