 // For a connection via I2C using Wire include
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SH1106Wire.h" //alis for `#include "SH1106Wire.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include <DHT.h>

//tabs
#include "Graphic_esp8266_dht22_oledi2c.h"


#define DHTPIN 0     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 11

// SSID and pass
const char* ssid     = "Indumentaria_Apicola";
const char* password = "Tesis2022";
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Initialize the OLED display using Wire library
SH1106Wire display(0x3c, 4, 5);
// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 10000;  

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
// Html
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP8266 DHT Server</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">%</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";
//
// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
  }
  return String();
}
void setup() {
  // Initialising the UI will init the display too.
  // Serial port for debugging purposes
  Serial.begin(115200);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  dht.begin(); // initialize dht
// // Connect to Wi-Fi
  /*WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }*/


  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h).c_str());
  });

  // Start server
  server.begin();
}

void displayTempHumid() {

// Reading temperature or humidity takes about 250 milliseconds!
// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
 float h = dht.readHumidity();

  // Read temperature as Celsius
float t = dht.readTemperature();

// Read temperature as Fahrenheit
// float f = dht.readTemperature(true);

// Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) ) {   // || isnan(f)
    display.clear(); // clearing the display
    display.drawString(5, 0, "Failed DHT");
    display.display();
    return;
  }
display.setColor(WHITE);

display.clear();
display.drawXbm(0, 15, 128, 35, img_esp8266);
display.display();
delay(3000);

display.clear();
display.drawXbm(47, 0, 32, 64, dht_logo);
display.display();
delay(3000);

display.clear();
display.setFont(ArialMT_Plain_16);
display.drawString(0, 0, "TEMPERATURA");
display.setFont(ArialMT_Plain_24);
display.drawString(0, 26, String(t) + " C");
display.drawXbm(100, 20, 20, 40, temp_logo);
display.display();
delay(5000);

display.clear();
display.setFont(ArialMT_Plain_16);
display.drawString(0, 0, "HUMEDAD");
display.setFont(ArialMT_Plain_24);
display.drawString(0, 26, String(h) + " %");
display.drawXbm(94, 20, 29, 40, humi_logo);
display.display();
delay(5000);

display.clear();
display.drawXbm(31, 0, 67, 64, ok_01);
display.display();
  delay(3000);
}

void loop() {
display.clear(); // clearing the display
displayTempHumid();
 unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
      Serial.println(t);
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      Serial.println(h);
    }
  }

}