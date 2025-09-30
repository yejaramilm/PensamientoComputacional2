/*  
   Universidad EAFIT
   Escuela de Ciencias Aplicadas e Ingeniería
   Ingeniería Agronómica
   Curso:    Pensamiento computacional II
   Programa: Ejemplo básico para uso académico de captura de humedad del suelo + RTC
   Por:      Yomin Jaramillo
*/

// ===== NUEVO: RTC por I2C =====
#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 rtc;
// ==============================

// Pines del sensor y LED
const int sensorPin = A0;   // Pin analógico conectado al sensor
const int ledPin = 13;      // LED integrado para indicar estado

// Valores de calibración (ajustar según pruebas)
const int airValue   = 1023;  // Lectura en aire (suelo seco)
const int waterValue = 400;   // Lectura en agua (suelo húmedo)

// ===== NUEVO: helper para 2 dígitos =====
void print2digits(int n) {
  if (n < 10) Serial.print('0');
  Serial.print(n);
}
// ========================================
void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  // Iniciar RTC
  Wire.begin();
  if (!rtc.begin()) {
    Serial.println("RTC no encontrado");
  }

  // Ajustar la hora si está detenido 
  if (!rtc.isrunning()) {
    Serial.println("RTC detenido; ajustando con hora de compilacion...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  // Leer valor crudo del sensor
  int moistureValue = analogRead(sensorPin);  // 0–1023

  // Convertir a porcentaje (0–100%)
  int moisturePercent = map(moistureValue, airValue, waterValue, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100); // Asegurar rango válido

  // ===== NUEVO: timestamp antes de la lectura =====
  DateTime now = rtc.now();
  Serial.print(now.year()); Serial.print('-');
  print2digits(now.month()); Serial.print('-');
  print2digits(now.day());   Serial.print(' ');
  print2digits(now.hour());  Serial.print(':');
  print2digits(now.minute());Serial.print(':');
  print2digits(now.second());
  Serial.print(" , ");

  // ================================

  // Mostrar en monitor serie (manteniendo tu formato)
  Serial.print("Humedad %: ");
  Serial.println(moisturePercent);

  // Encender LED según humedad
  if (moistureValue > 700) {        // Umbral ajustable: >700 = seco
    digitalWrite(ledPin, HIGH);     // LED encendido = suelo seco
  } else {
    digitalWrite(ledPin, LOW);      // LED apagado = suelo húmedo
  }

  delay(500);  // Esperar 0.5 segundos antes de la siguiente lectura
}
