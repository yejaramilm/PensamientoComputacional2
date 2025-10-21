#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

RTC_DS1307 rtc;

const int soilPin = A0;
const int ldrPin  = A1;
const int ledPin  = 13;
const int chipSelect = 4;  // <-- revisa este valor según tu módulo

const int airValue = 1023;
const int waterValue = 400;

File dataFile;
const char *filename = "DATA.CSV";

void setup() {
  Serial.begin(9600);
  pinMode(53, OUTPUT);
  digitalWrite(53, HIGH);
  pinMode(ledPin, OUTPUT);

  Wire.begin();
  dht.begin();

  if (!rtc.begin()) Serial.println("RTC no encontrado!");
  if (!rtc.isrunning()) {
    Serial.println("RTC detenido, ajustando...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  Serial.print("Inicializando SD...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Error al inicializar SD!");
    while (1);
  }
  Serial.println("SD lista.");

  dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println("Fecha,Hora,HumedadSuelo%,Luz_LDR,TempDHT11,HumedadDHT11");
    dataFile.close();
  }
}

void loop() {
  int soilRaw = analogRead(soilPin);
  int ldrValue = analogRead(ldrPin);
  float humidityAir = dht.readHumidity();
  float tempAir = dht.readTemperature();

  int soilPercent = map(soilRaw, airValue, waterValue, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  DateTime now = rtc.now();

  char dataLine[80];
  snprintf(dataLine, sizeof(dataLine), 
           "%04d-%02d-%02d,%02d:%02d:%02d,%d,%d,%.1f,%.1f",
           now.year(), now.month(), now.day(),
           now.hour(), now.minute(), now.second(),
           soilPercent, ldrValue, tempAir, humidityAir);

  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataLine);
    dataFile.close();
    Serial.println(dataLine);
  } else {
    Serial.println("⚠️ Error escribiendo en SD");
  }

  delay(2000);
}
