/*  Universidad EAFIT
    Escuela de Ciencias Aplicadas e Ingeniería
    Ingeniería Agronómica
    Curso:    Pensamiento conputacional II
    Programa: Ejemplo básicos para uso académico de camptura de temperatura
    Por:      Yomin Jaramillo
*/
// Define sensor pins
const int sensorPin = A0;  // Analog pin connected to the sensor
const int ledPin = 13;     // Onboard LED for indication
const int airValue=1023;
const int waterValue=400;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);      // Start serial communication
}

void loop() {
  int moistureValue = analogRead(sensorPin);  // Read the analog value 0-1023
  
  int moisturePercent = map(moistureValue, airValue, waterValue, 0, 100);
  // Asegura que esté entre 0 y 100
  moisturePercent = constrain(moisturePercent, 0, 100);
  Serial.print("Humedad %: ");
  Serial.println(moisturePercent);

  // Simple LED indicator
  if (moistureValue < 400) {  // Adjust threshold as needed
    digitalWrite(ledPin, HIGH);  // Turn on LED if soil is dry
  } else {
    digitalWrite(ledPin, LOW);   // Turn off LED if soil is moist
  }

  delay(500);  // Wait for 500 second before next reading
}
