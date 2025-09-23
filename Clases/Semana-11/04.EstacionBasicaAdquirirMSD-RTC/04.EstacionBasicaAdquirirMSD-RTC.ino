/*  Universidad EAFIT
    Escuela de Ciencias Aplicadas e Ingeniería
    Ingeniería Agronómica
    Curso:    Pensamiento conputacional II
    Programa: Ejemplo para guardar datos en una Micro SD con fecha en tiempo reloj usando un reloj digital
    Por:      Luis Fernando Londoño Londoño
*/

#include <DHT.h>
#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include "RTClib.h"

#define DHTPIN 2
#define DHTTYPE DHT11
#define mSDPin 9

RTC_DS3231 rtc;

//Defino nombre del archivo para la MicroSD
File logFile;
String fileName = "data"+String(random(9999)) + ".csv";

DHT dht(DHTPIN, DHTTYPE);

//Variables
float h = 0;
float t = 0;
float f = 0;
int tiempoActual = 0;
int tiempoEspera = 5;
int tiempoConteo = 0;
DateTime fecha;

void setup() {
    Serial.begin(9600);
    dht.begin();
   
    tiempoActual = millis()/1000;
    tiempoConteo = tiempoActual - 1;    
    
    //Serial.println(F("Iniciando SD ..."));
    if (!SD.begin(mSDPin))
    {
      Serial.println(F("Error iniciando SD"));
      return;
    }
    //Serial.println(F("Iniciado correctamente"));

    while (SD.exists(fileName))
    {
      fileName = fileName.substring(0, 4) + String(random(9999)) + ".csv";
    }
    //Serial.println("Archivo:"+fileName);

    if  (!rtc.begin()) {
        Serial.println(F("No se puede iniciar el reloj"));
        while (1);
   }

   logFile = SD.open(fileName, FILE_WRITE); 
  
   // Si se ha perdido la corriente, fijar fecha y hora

   
   if (rtc.lostPower()) {
      // Fijar a fecha y hora de compilacion
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   } 
    
}

String leerSensor()
{
  h = dht.readHumidity();         // Leemos la humedad relativa
  t = dht.readTemperature();      // Leemos la temperatura en grados centígrados (por defecto)
  f = dht.readTemperature(true);  // Leemos la temperatura en grados Fahrenheit
  
  // Comprobamos si ha habido algún error en la lectura
  if (isnan(h) || isnan(t) || isnan(f)) {
     Serial.println("Error obteniendo los datos del sensor DHT11");
     return "";
  }
  
  float hif = dht.computeHeatIndex(f, h);         // Calcular el índice de calor en Fahrenheit
  float hic = dht.computeHeatIndex(t, h, false);  // Calcular el índice de calor en grados centígrados
  String datoSensor = String(t)+";"+String(h)+";"+String(f)+";"
                      +String(hic)+";"+String(hif);
  return datoSensor;
}

String strFecha(DateTime fecha)
{
  String strFecha = String(fecha.year(), DEC) + '/' + String(fecha.month(), DEC) + '/' + String(fecha.day(), DEC) + " " + String(fecha.hour(), DEC) + ':' + String(fecha.minute(), DEC) + ':' + String(fecha.second(), DEC);
  return strFecha;
}

void guardarMicroSD(String datosSensados, DateTime fecha)
{
  if (logFile) { 
    logFile.println(strFecha(fecha)+";"+datosSensados);
  } 
  else {
    Serial.println("Error al abrir el archivo: " + fileName);
  } 
}

void loop() {
  fecha = rtc.now();

  if (tiempoActual > tiempoConteo + tiempoEspera){
      String datosSensados = leerSensor();
      //Serial.println(strFecha(fecha)+" --> "+datosSensados);
      Serial.println(datosSensados);
      guardarMicroSD(datosSensados,fecha );
      tiempoConteo = tiempoActual - 1;      
    }
    else {
      tiempoActual = millis()/1000;
    }
 }
