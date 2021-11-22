// Library für WiFi-Verbindung
#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"


// Daten des WiFi-Netzwerks
const char* ssid     = "Obi-Wan";
const char* password = "n9BcVp019DkF@PYdYwz4mRJ8";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


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
    <h1>ESP8266 Universal IR Web Server</h1>
    
    <p>
    <span>GPIO D6 -> LED_PIN_STATE - State</span> 
    <span id="LED_PIN_STATE">%LED_PIN_STATE%</span>
    <sup class="units">Placeholder</sup>
    </p> 
    
    <div>%BUTTONPLACEHOLDER%</div>
    
    <p>Currently DEV Phase with NEC Protokoll.</p>
    </body> 

  
  
  <script>
  
  function Button_GPIO_D6()
  {
    if(LED_PIN_STATE == 'ON')
    {
      GPIO_D6('OFF');
    }
    else if(LED_PIN_STATE == 'OFF')
    {
      GPIO_D6('ON');  
    }
  }  
       
  function GPIO_D6(STATE)
  {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "/set?GPIO_D6/"+STATE, true);
    xhttp.send();
  }
  
  

  setInterval(function ( ){
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function()
    {
    if (this.readyState == 4 && this.status == 200)
      {
        document.getElementById("LED_PIN_STATE").innerHTML = this.responseText;
        globalThis.LED_PIN_STATE = this.responseText;
      }
    };
    xhttp.open("GET", "/get?GPIO_D6", true);
    xhttp.send();
  }, 100 ) ;

</script>
</html>
)rawliteral";

const char button_html[] PROGMEM = R"rawliteral(
<button class="button" onclick=Button_GPIO_D6()>LED Button</button>
<button class="button" onmousedown="GPIO_D6('ON');" ontouchstart="GPIO_D6('ON');" onmouseup="GPIO_D6('OFF');" ontouchend="GPIO_D6('OFF');">LED PUSHBUTTON</button>
)rawliteral";



// Auxiliar variables naming sceme
// Assign output variables to GPIO pins
const int LED_PIN = D6;

// Auxiliar variables to store the current output state
String LED_PIN_STATE = "OFF";

// Auxiliar variables to store URL
String LED_PIN_URL = "GPIO_D6";
String LED_PIN_URL_ON = LED_PIN_URL + "/ON";
String LED_PIN_URL_OFF = LED_PIN_URL + "/OFF";



String processor(const String& var){
  //Serial.println(var);
  if(var == "LED_PIN_STATE"){
    return LED_PIN_STATE;
  }

  else if (var ==  "BUTTONPLACEHOLDER"){
    return button_html;
  }
  return String();
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Initialize the output variables as outputs
  pinMode(LED_PIN, OUTPUT);
  
  // Set outputs to LOW
  digitalWrite(LED_PIN, LOW);
  


  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());



  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  


  // Send a GET request to <IP>/get?param=<message> OR <IP>/get?param
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request){

    if(request->hasParam(LED_PIN_URL)){
      request->send_P(200, "text/plain", LED_PIN_STATE.c_str());
    }

  });

  server.on("/set", HTTP_GET, [] (AsyncWebServerRequest *request){
  
    if (request->hasParam(LED_PIN_URL_ON)){
      digitalWrite(LED_PIN, HIGH);
      LED_PIN_STATE = "ON";
      request->send(200, "text/plain", "ok");
    }

    else if (request->hasParam(LED_PIN_URL_OFF)){
      digitalWrite(LED_PIN, LOW);
      LED_PIN_STATE = "OFF";
      request->send(200, "text/plain", "ok");
    }
  });



  server.onNotFound(notFound);
  // Start server
  server.begin();
  }

  void loop(){}