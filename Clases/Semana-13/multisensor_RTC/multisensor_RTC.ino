/*  
   Universidad EAFIT
   Escuela de Ciencias Aplicadas e Ingeniería
   Ingeniería Agronómica
   Curso:    Pensamiento computacional II
   Programa: Ejemplo básico para uso académico de captura de humedad del suelo + RTC + MQ135
   Por:      Yomin Jaramillo
*/

// ===== NUEVO: RTC por I2C =====
#include <Wire.h>               // Librería I2C
#include "RTClib.h"             // Librería del RTC (Adafruit RTClib)
RTC_DS1307 rtc;                 // Instancia del reloj DS1307
// ==============================

// Pines de sensores y LED
const int sensorPin  = A0;      // Pin analógico del sensor de humedad de suelo
const int mq135Pin   = A1;      // <-- NUEVO: Pin analógico del MQ135 (gas/aire)
const int ledPin     = 13;      // LED integrado para indicar estado

// Valores de calibración (ajustar según pruebas de tu suelo/sensor)
const int airValue   = 1023;    // Lectura en aire (suelo seco)
const int waterValue = 400;     // Lectura en agua (suelo húmedo)

// Helper para imprimir con 2 dígitos (p. ej., 07)
void print2digits(int n) {
  if (n < 10) Serial.print('0');  // Anteponer 0 si es un dígito
  Serial.print(n);                // Imprimir el número
}

void setup() {
  pinMode(ledPin, OUTPUT);      // LED como salida
  Serial.begin(9600);           // Puerto serie a 9600 baudios

  Wire.begin();                 // Iniciar bus I2C
  if (!rtc.begin()) {           // Verificar presencia del RTC
    Serial.println("RTC no encontrado");
  }

  // Si el RTC está detenido, ajustarlo con fecha/hora de compilación (Opción A)
  if (!rtc.isrunning()) {
    Serial.println("RTC detenido; ajustando con hora de compilacion...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Arranca el reloj
  }
}

void loop() {
  // ---- Lecturas analógicas ----
  int moistureValue = analogRead(sensorPin);   // Lectura cruda 0–1023 del sensor de humedad
  int mq135Raw      = analogRead(mq135Pin);    // <-- NUEVO: Lectura cruda 0–1023 del MQ135

  // ---- Conversión de humedad a porcentaje ----
  int moisturePercent = map(moistureValue, airValue, waterValue, 0, 100); // Mapear al 0–100%
  moisturePercent = constrain(moisturePercent, 0, 100);                   // Limitar al rango válido

  // ---- Timestamp (YYYY-MM-DD HH:MM:SS , ) ----
  DateTime now = rtc.now();                // Obtener fecha y hora actuales del RTC
  Serial.print(now.year()); Serial.print('-');     // Año-
  print2digits(now.month()); Serial.print('-');    // Mes-
  print2digits(now.day());   Serial.print(' ');    // Día␣
  print2digits(now.hour());  Serial.print(':');    // Hora:
  print2digits(now.minute());Serial.print(':');    // Min:
  print2digits(now.second());                      // Seg
  Serial.print(" , ");                              // Separador visual

  // ---- Salida formateada en una sola línea ----
  Serial.print("Humedad %: ");              // Clave de humedad
  Serial.print(moisturePercent);            // Valor de humedad (antes era println → CAMBIO mínimo)
  Serial.print(" , MQ135_raw: ");           // <-- NUEVO: Clave para MQ135 en crudo
  Serial.println(mq135Raw);                 // <-- NUEVO: Valor del MQ135 y FIN de línea

  // ---- Lógica del LED según humedad del suelo ----
  if (moistureValue > 700) {                // Umbral ajustable: >700 se considera seco
    digitalWrite(ledPin, HIGH);             // LED encendido = suelo seco
  } else {
    digitalWrite(ledPin, LOW);              // LED apagado  = suelo húmedo
  }

  delay(500);                               // Esperar 0.5 s entre mediciones
}
