// Library für WiFi-Verbindung
#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"

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

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html>
  <head>
    <title>ESP Web Server</title>
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">
    <link rel=\"icon\" href=\"data:,\">
    
    <style>
      html 
      { 
        font-family: Helvetica;
        display: inline-block;
        margin: 0px auto;
        text-align: center;
      }
      .button 
      { 
        background-color: #195B6A;
        border: none;
        color: white;
        padding: 16px 40px;
        text-decoration: none;
        font-size: 30px;
        margin: 2px;
        cursor: pointer;
      }                 
      .button2 
      {
        background-color: #77878A;
        border: none;
        color: white;
        padding: 16px 40px;
        text-decoration: none;
        font-size: 30px;
        margin: 2px;
        cursor: pointer;
      }
    </style> 
  </head>
  
  <body>
    
    <div><img src="https://cdn3.iconfinder.com/data/icons/streamline-icon-set-free-pack/48/Streamline-16-512.png" width="256" height="256" ></div>
    <h1>ESP8266 Universal IR Web Server</h1>
    
    <div id="BUTTONSTATEPLACEHOLDER">%BUTTONSTATEPLACEHOLDER%</div>
    <div id="BUTTONPLACEHOLDER">%BUTTONPLACEHOLDER%</div>
    
    
    <p>Currently DEV Phase with NEC Protokoll.</p>
    </body> 

  
  
  <script>
  
  // Server-Sent Events (SSE)  
  if (!!window.EventSource) 
  {
    var update_event = new EventSource('/events');
    
    update_event.addEventListener('open', function(e)
    {
      console.log("Events Connected");
    }, false);

    update_event.addEventListener('error', function(e)
    {
      if (e.target.readyState != EventSource.OPEN)
      {
        console.log("Events Disconnected");
      }
    }, false);
 
    update_event.addEventListener('message', function(e)
    {
      console.log("message", e.data);
    }, false);
 

    //Events
    // FOR every Button with a State Event has to be created

    update_event.addEventListener('BUTTON_URL', function(e)
    {
      console.log("BUTTON_URL", e.data);
      var data = e.data;
      
      update_event.addEventListener(data, function(e)
      {
        console.log("BUTTON", e.data);
        document.getElementById(data).innerHTML = e.data;
      }, false);

    }, false);
  }  

  
  function EventHandler(URL)
  {
    update_event.addEventListener(URL, function(e)
    {
      console.log("EventHandler", e.data);
      document.getElementById(URL).innerHTML = e.data;
    }, false);
  }



  // HTML inject Method with only the URL
  function Button(URL)
  {
    //EventHandler(URL);
    // 1st Get State
    GET_STATE(function(_STATE)  
    {
      //console.debug(_STATE);
      
      // 2nd Set Buttons
      ButtonHandler(function(response)
      {
        //console.debug(response);

        // 3nd Update State
        GET_STATE(function(GET_STATE)
        {
        //console.debug(GET_STATE);
        UPDATE_STATE(URL, GET_STATE);
        }, URL);

      }, URL, _STATE);

    }, URL);
  }  

  function ButtonHandler(callback, URL, STATE)
  {
    if(STATE === "ON")
      {
        var xhttp = new XMLHttpRequest();
        
        xhttp.onload = function()
        {
          callback(xhttp.responseText);
        };

        xhttp.open("GET", "/set?"+URL+"OFF", true);
        xhttp.send();  
      }
      else if(STATE === "OFF")
      {
        var xhttp = new XMLHttpRequest();

        xhttp.onload = function()
        {
          callback(xhttp.responseText);
        };

        xhttp.open("GET", "/set?"+URL+"ON", true);
        xhttp.send();
      }
  }

  function GET_STATE(callback, URL)
  {
    var xhttp = new XMLHttpRequest();
    xhttp.onload = function()
    {
      callback(xhttp.responseText);
    };

    xhttp.open("GET", "/get?"+URL, true);
    xhttp.send();
  }

  function UPDATE_STATE(URL, STATE)
  {
    document.getElementById(URL).innerHTML = STATE;
  }


</script>
</html>
)rawliteral";

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
String button_html = "<div>GPIO D6 -> LED_PIN_STATE - State</div><div id=\"" + LED_PIN_URL + "\">%" + LED_PIN_URL + "%</div><button class=\"button\" onclick=\"Button('" + LED_PIN_URL + "');\">LED Button</button>";

// Functions
String processor(const String &var)
{
  //Serial.println(var);
  if (var == LED_PIN_URL)
  {
    return LED_PIN_STATE;
  }
  if (var == "BUTTONPLACEHOLDER")
  {
    return button_html;
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
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html, processor); });

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


  if (LED_PIN_STATE != last_LED_PIN_STATE)
  {
    Serial.printf("LED PIN STATE = %s\n", LED_PIN_STATE);
    // events.send(String(LED_PIN_STATE).c_str(),"GPIO_D6",millis());
    // events.send(function e.data, EventListenerName, millis());

    // Send Event for every Button to initialize EventListener for State
    //events.send(String(LED_PIN_URL).c_str(), "BUTTON_URL", millis()); ^in on connect -> less overhead
    // Send Event to Update State
    events.send(String(LED_PIN_STATE).c_str(), String(LED_PIN_URL).c_str(), millis());

    last_LED_PIN_STATE = LED_PIN_STATE;
  }

  if ((millis() - lastTime) > timerDelay)
  {
    // Send Events to the Web Server Ping
    events.send("ping", NULL, millis());

    lastTime = millis();
  }


}