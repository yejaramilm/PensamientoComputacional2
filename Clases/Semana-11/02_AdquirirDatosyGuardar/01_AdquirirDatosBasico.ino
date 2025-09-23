
/*  
   Universidad EAFIT
   Escuela de Ciencias Aplicadas e Ingeniería
   Ingeniería Agronómica
   Curso:    Pensamiento computacional II
   Programa: Ejemplo básico para uso académico de captura de humedad del suelo
   Por:      Yomin Jaramillo
*/

// Pines del sensor y LED
const int sensorPin = A0;   // Pin analógico conectado al sensor
const int ledPin = 13;      // LED integrado para indicar estado

// Valores de calibración (ajustar según pruebas)
const int airValue   = 1023;  // Lectura en aire (suelo seco)
const int waterValue = 400;   // Lectura en agua (suelo húmedo)

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);   // Inicia comunicación serial
}

void loop() {
  // Leer valor crudo del sensor
  int moistureValue = analogRead(sensorPin);  // 0–1023

  // Convertir a porcentaje (0–100%)
  int moisturePercent = map(moistureValue, airValue, waterValue, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100); // Asegurar rango válido

  // Mostrar en monitor serie
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
