/*********
  Agustín Garrido

*********/

// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Temporizador
unsigned long marcaTiempoDate = 0;
unsigned long tiempoRefreshDate = 1000;
 
// Variables almacena tiempo millis
int dias;
int horas;
int minutos;
int segundos;
// Cadena para almacenar texto formateado
char cadenaTiempo[16];
// Definir constantes
#define ANCHO_PANTALLA 128 // ancho pantalla OLED
#define ALTO_PANTALLA 64 // alto pantalla OLED
 
// Objeto de la clase Adafruit_SSD1306
Adafruit_SSD1306 display(128, 64, &Wire, -1);
const char* ssid     = "ESP8266-Access-Point";
const char* password = "123456789";

#define DHTPIN 5     // Digital pin connected to the DHT sensor

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 10000;  

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
/*
  Función que convierte millis() a segundos, minutos, horas y días
  Almacena la información en variables globales
*/
void millisToTiempo(unsigned long valMillis) {
  // Se obtienen los segundos
  valMillis = valMillis / 1000;
 
  segundos = valMillis % 60; // se divide entre segundos por minuto y te quedas con el resto
  minutos = valMillis / 60; // Se convierte a minutos
  minutos = minutos % 60; // se divide entre minutos por hora y te quedas con el resto
  horas = (valMillis / 60) / 60; // Se convierte en horas
  horas = horas % 24; // se divide entre horas al día y te quedas con el resto
  dias = ((valMillis / 60) / 60) / 24; // Se convierte en días
#ifdef __DEBUG__
  Serial.print("Segundos = ");
  Serial.println(valMillis);
  Serial.print(dias);
  Serial.print(":");
  Serial.print(horas);
  Serial.print(":");
  Serial.print(minutos);
  Serial.print(":");
  Serial.println(segundos);
#endif
}
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  dht.begin();
   // Iniciar pantalla OLED en la dirección 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    #ifdef __DEBUG__
    Serial.println("No se encuentra la pantalla OLED");
    #endif
    while (true);
  }
   
    // Iniciar pantalla OLED en la dirección 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
#ifdef __DEBUG__
    Serial.println("No se encuentra la pantalla OLED");
#endif
    while (true);
  }
 
  // Limpir buffer
  display.clearDisplay();
 
  // Tamaño del texto
  display.setTextSize(1);
  // Color del texto
  display.setTextColor(SSD1306_WHITE);
  // Posición del texto
  display.setCursor(10, 32);
  //Activar página de código 437
  display.cp437(true);
  // Escribir el carácter ¡ dos veces
  display.write(173);
  display.write(173);
  // Escribir texto
  display.println("Hola mundo!!");
  
  // Enviar a pantalla
  display.display();
  delay(2000);
 
  // Mover texto de izquierda a derecha
  display.startscrollright(0x00, 0x0F);
  delay(5000);
  display.stopscroll();
  
  // Mover texto de derecha a izquierda
  display.startscrollleft(0x00, 0x0F);
  delay(5000);
  display.stopscroll();
 
  // Mover texto en diagonal hacia la derecha
  display.startscrolldiagright(0x00, 0x07);
  delay(5000);
  display.stopscroll();
 
  // Mover texto en diagonal hacia la izquierda
  display.startscrolldiagleft(0x00, 0x07);
  delay(5000);
  display.stopscroll();
 
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
 
void loop(){  
  // Protección overflow
  if (millis() < marcaTiempoDate) {
    marcaTiempoDate = millis();
  }
 
  // Comprobar is hay que actualizar temperatura
  if (millis() - marcaTiempoDate >= tiempoRefreshDate)
  {
    // Actualizar variables de tiempo
    millisToTiempo(millis());
    // Componer cadena con la información del tiempo formateada
    sprintf(cadenaTiempo, "%02d:%02d:%02d:%02d", dias, horas, minutos, segundos);
    // Marca de tiempo
    marcaTiempoDate = millis();
  }
 
  // Limpiar buffer pantalla
  display.clearDisplay();
  // Dibujar línea horizontal
  display.drawLine(0, 18, display.width(), 18, SSD1306_WHITE);
 
  // Dibujar texto tiempo
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(cadenaTiempo);
 
  // Enviar a pantalla
  display.display();
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