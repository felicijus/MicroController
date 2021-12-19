// Library für WiFi-Verbindung
#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"

#include <FS.h>
#include <LittleFS.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>

int LED_PIN(D6);
IRsend IRTx(D8);
IRrecv IRRx(D7);

decode_results results;

// Daten des WiFi-Netzwerks
const char *ssid = "Felix Schneider";  //Obi-Wan
const char *password = "helloxilef";  //n9BcVp019DkF@PYdYwz4mRJ8

const char *hostname = "ESP8266";

//IPAddress local_ip(10, 0, 0, 250);
//IPAddress gateway(10, 0, 0, 1);
//IPAddress subnet(255, 255, 254, 0);
//IPAddress dns(1, 1, 1, 1);


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;

// Functions
String processor(const String &var)
{

  return String();
}

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

// SETUP
void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(LED_PIN,OUTPUT);

  // Initialize LittleFS
  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // IR
  IRTx.begin();
  IRRx.enableIRIn();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  //WiFi.config(local_ip,gateway,subnet,dns);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  WiFi.hostname(hostname);
  Serial.printf("%s\n", WiFi.hostname().c_str());
  
  //WiFi.softAP(ssid, password);



  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", String(), false, processor); });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/style.css", "text/css"); });

  // Route to load script.js file
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/script.js", "text/css"); });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client)
                   {
                     if (client->lastId())
                     {
                       Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
                     }
                     // Send Event with message "Initialized", id current millis
                     // and set reconnect delay to 1 second
                     client->send("Event Handler for this Client initialized", NULL, millis(), 10000); });
  server.addHandler(&events);

  // Send a GET request to <IP>/get?param=<message> OR <IP>/get?param
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {

  });


  server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    int params = request->params();
    for (int i = 0; i < params; i++)
    {
      AsyncWebParameter *p = request->getParam(i);
      
      Serial.printf("SET[%s]: %s\n", p->name().c_str(), p->value().c_str());
        
      unsigned long code = strtol(p->value().c_str(), NULL, 16);

      digitalWrite(LED_PIN,HIGH);
      IRTx.sendNEC(code);
      digitalWrite(LED_PIN,LOW);
      
    }
      request->send(200, "text/plain", "ok");
    });

    server.onNotFound(notFound);
    // Start server
    server.begin();
}

void loop()
{
    /*if (IRRx.decode(&results))
    {
      Serial.println(results.value, HEX);
      IRRx.resume();
    }*/

    if ((millis() - lastTime) > timerDelay)
    {
      // Send Events to the Web Server Ping
      events.send("ping", NULL, millis());

      lastTime = millis();
    }
}