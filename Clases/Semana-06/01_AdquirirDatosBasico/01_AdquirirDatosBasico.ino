/*  Universidad EAFIT
    Escuela de Ciencias Aplicadas e Ingeniería
    Ingeniería Agronómica
    Curso:    Pensamiento conputacional II
    Programa: Ejemplo básicos para uso académico de camptura de temperatura
    Por:      Yomin Jaramillo
*/

//Declaración de librerías
#include <DHT.h>
#include <SPI.h>
#include <Wire.h>

//Definición de constantes
#define DHTPIN 9
#define ALARMA 13
#define DHTTYPE DHT11
#define T_ESPERA 5

//Definición de Variables
DHT dht(DHTPIN, DHTTYPE);
int tiempoActual = 0;
int tiempoConteo = 0;

float h;
float t;
float f;
int alerta = 25; // Cuando la tempreatura sea mauyor 25, prende LED

// Configuración inicial
void setup() {
  Serial.begin(9600);
  dht.begin();
  tiempoActual = millis()/1000;
  tiempoConteo = tiempoActual;
  digitalWrite( ALARMA, LOW);
}
  
// Ciclo del programa
void loop() {
  if (tiempoActual > tiempoConteo + T_ESPERA){
      h = dht.readHumidity();     // Leemos la humedad relativa
      t = dht.readTemperature();  // Leemos la temperatura en grados centígrados (por defecto)
      f = dht.readTemperature(true);  // Leemos la temperatura en grados Fahrenheit

      // Comprobamos si ha habido algún error en la lectura
      if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println("Error obteniendo los datos del sensor DHT11");
        return;
      }
      
      if (t > alerta){
          digitalWrite(ALARMA, HIGH); 
      }
    
      float hif = dht.computeHeatIndex(f, h); // Calcular el índice de calor en Fahrenheit
      float hic = dht.computeHeatIndex(t, h, false); // Calcular el índice de calor en grados centígrados
    
      // Armamos string para ser enviado al serial
      String dato_sensor = String(t)+";"+String(h)+";"+String(f)+";"+String(hic)+";"+String(hif)+";"+String(0)+";"+String(0);
      Serial.println(dato_sensor);
    
      tiempoConteo = tiempoActual;
  }
  else {
    digitalWrite(ALARMA, LOW);
    tiempoActual = millis()/1000;
  }
}
