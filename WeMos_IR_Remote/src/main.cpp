// Library für WiFi-Verbindung
#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"

#include <FS.h>
#include <LittleFS.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>

IRsend IRTx(D8);
IRrecv IRRx(D7);

decode_results results;

// Daten des WiFi-Netzwerks
const char *ssid = "Obi-Wan";
const char *password = "n9BcVp019DkF@PYdYwz4mRJ8";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;




// Auxiliar variables naming sceme
// Assign output variables to GPIO pins
const int LED_PIN = D6;

// Auxiliar variables to store the current output state
String LED_PIN_STATE = "OFF";
String last_LED_PIN_STATE; // For Event Calling

// Auxiliar variables to store URL
String LED_PIN_URL = "GPIO_D6";
String LED_PIN_URL_ON = LED_PIN_URL + "ON";
String LED_PIN_URL_OFF = LED_PIN_URL + "OFF";

// Buttons

// Functions
String processor(const String &var)
{
  //Serial.println(var);
  if (var == LED_PIN_URL)
  {
    return LED_PIN_STATE;
  }

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

  // Initialize LittleFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Initialize the output variables as outputs
  pinMode(LED_PIN, OUTPUT);

  // Set outputs to LOW
  digitalWrite(LED_PIN, LOW);


  //IR
  IRTx.begin();
  IRRx.enableIRIn();



  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });

  
  

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client)
                   {
                     if (client->lastId())
                     {
                       Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
                     }
                     // Send Event with message "Initialized", id current millis
                     // and set reconnect delay to 1 second
                     client->send("Event Handler for this Client initialized", NULL, millis(), 10000);

                     // Send Event for every Button to initialize EventListener for State
                     client->send(String(LED_PIN_URL).c_str(), "BUTTON_URL", millis());
                     //
                     //
                   });
  server.addHandler(&events);

  // Send a GET request to <IP>/get?param=<message> OR <IP>/get?param
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (request->hasParam(LED_PIN_URL))
              {
                request->send_P(200, "text/plain", LED_PIN_STATE.c_str());
              }
            });

  server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (request->hasParam(LED_PIN_URL_ON))
              {
                digitalWrite(LED_PIN, HIGH);
                LED_PIN_STATE = "ON";
                
                IRTx.sendNEC(0x3EC1C23D);


                ////
                request->send(200, "text/plain", "ok");
              }

              else if (request->hasParam(LED_PIN_URL_OFF))
              {
                digitalWrite(LED_PIN, LOW);
                LED_PIN_STATE = "OFF";
                request->send(200, "text/plain", "ok");
              }
            });

  server.on("/test", HTTP_GET, [](AsyncWebServerRequest *request)
  {

  });
  
  server.onNotFound(notFound);
  // Start server
  server.begin();
}

void loop()
{
  
  uint32 IR_TEMP;

  if (IRRx.decode(&results)){
    Serial.println(results.value, HEX);
    IRRx.resume();
  }


  if ((millis() - lastTime) > timerDelay)
  {
    // Send Events to the Web Server Ping
    events.send("ping", NULL, millis());

    lastTime = millis();
  }


}