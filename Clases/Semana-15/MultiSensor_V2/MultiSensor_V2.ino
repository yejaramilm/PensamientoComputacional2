/*  
   Universidad EAFIT
   Escuela de Ciencias Aplicadas e Ingeniería
   Ingeniería Agronómica
   Curso:    Pensamiento computacional II
   Programa: Ejemplo básico para uso académico de captura de humedad del suelo + RTC + DHT11 + Fotocelda
   Por:      Yomin Jaramillo
*/

// ===== Librerías =====
#include <Wire.h>               // Comunicación I2C
#include "RTClib.h"             // RTC (Adafruit RTClib)
#include "DHT.h"                // Librería del sensor DHT11

// ===== Configuración de sensores =====
#define DHTPIN 2                // Pin digital donde se conecta el DHT11
#define DHTTYPE DHT11           // Tipo de sensor DHT
DHT dht(DHTPIN, DHTTYPE);       // Instancia del sensor DHT11

RTC_DS1307 rtc;                 // Instancia del reloj DS1307

// ===== Pines de sensores y LED =====
const int soilPin = A0;         // Pin analógico del sensor de humedad de suelo
const int ldrPin  = A1;         // Pin analógico de la fotocelda (LDR)
const int ledPin  = 13;         // LED integrado

// ===== Calibración del sensor de humedad =====
const int airValue   = 1023;    // Lectura en aire (suelo seco)
const int waterValue = 400;     // Lectura en agua (suelo húmedo)

// ===== Función para imprimir con 2 dígitos =====
void print2digits(int n) {
  if (n < 10) Serial.print('0');
  Serial.print(n);
}

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  // ---- Inicializar RTC ----
  Wire.begin();
  if (!rtc.begin()) {
    Serial.println("RTC no encontrado");
  }
  if (!rtc.isrunning()) {
    Serial.println("RTC detenido; ajustando con hora de compilacion...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // ---- Inicializar DHT11 ----
  dht.begin();
}

void loop() {
  // ---- Lecturas analógicas ----
  int soilRaw  = analogRead(soilPin);          // Lectura cruda 0–1023 del sensor de humedad
  int ldrValue = analogRead(ldrPin);           // Lectura cruda de la fotocelda

  // ---- Lecturas del DHT11 ----
  float humidityAir = dht.readHumidity();      // Humedad ambiental %
  float tempAir     = dht.readTemperature();   // Temperatura °C

  // ---- Conversión de humedad del suelo a porcentaje ----
  int soilPercent = map(soilRaw, airValue, waterValue, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  // ---- Timestamp (YYYY-MM-DD HH:MM:SS , ) ----
  DateTime now = rtc.now();
  Serial.print(now.year()); Serial.print('-');
  print2digits(now.month()); Serial.print('-');
  print2digits(now.day()); Serial.print(' ');
  print2digits(now.hour()); Serial.print(':');
  print2digits(now.minute()); Serial.print(':');
  print2digits(now.second());
  Serial.print(" , ");

  // ---- Salida formateada ----
  Serial.print("HumedadSuelo%: ");
  Serial.print(soilPercent);
  Serial.print(" , Luz_LDR: ");
  Serial.print(ldrValue);
  Serial.print(" , TempDHT11: ");
  Serial.print(tempAir);
  Serial.print(" °C , HumedadDHT11: ");
  Serial.print(humidityAir);
  Serial.println(" %");

  // ---- LED indicador según humedad del suelo ----
  if (soilRaw > 700) {
    digitalWrite(ledPin, HIGH);   // Seco → LED ON
  } else {
    digitalWrite(ledPin, LOW);    // Húmedo → LED OFF
  }

  delay(1000);  // Esperar 1 s entre mediciones
}
