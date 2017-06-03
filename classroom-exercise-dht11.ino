// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include <SPI.h>
#include <SD.h>
#include "DHT.h"
#include "RTClib.h"

#define DHTPIN 5

#define DHTTYPE DHT11

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);
File logFile;
RTC_PCF8523 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void error(const char *err) {
  Serial.println(err);
    
  while (1) {
    delay(10);
  }
}

void setup() {
  Serial.begin(9600);

  while (!Serial && millis() < 5 * 1000) {
  }
  
  Serial.println("classroom-exercise-dht11");

  if (!rtc.begin()) {
    error("RTC is unavailable!");
  }

  if (! rtc.initialized()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  
  if (!SD.begin(10)) {
    error("No SD Card");
  }
  
  logFile = SD.open("DATA.CSV", FILE_WRITE);
  if (!logFile) {
    error("Unable to open DATA.CSV");
  }

  dht.begin();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print(" ");

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");
  
  logFile.print(now.year(), DEC);
  logFile.print('/');
  logFile.print(now.month(), DEC);
  logFile.print('/');
  logFile.print(now.day(), DEC);
  logFile.print(" ");
  logFile.print(now.hour(), DEC);
  logFile.print(':');
  logFile.print(now.minute(), DEC);
  logFile.print(':');
  logFile.print(now.second(), DEC);
  logFile.print(",");

  logFile.print(h);
  logFile.print(",");
  logFile.print(t);
  logFile.print(",");
  logFile.print(f);
  logFile.println("");
  logFile.flush();
}
