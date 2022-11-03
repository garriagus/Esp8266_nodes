 // For a connection via I2C using Wire include
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SH1106Wire.h" //alis for `#include "SH1106Wire.h"`

#include "Graphic_esp8266_dht22_oledi2c.h"

#include <DHT.h>

#define DHTPIN 0     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 11


// Initialize the OLED display using Wire library
SH1106Wire display(0x3c, 4, 5);

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  dht.begin(); // initialize dht
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
display.drawString(0, 0, "HUMIDITY");
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

delay(2000);
}