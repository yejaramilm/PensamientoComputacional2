// Pines
int ledPin = 50;
int buttonPin = 48;

// Variable para guardar el estado del botón
int buttonState = 0;

void setup() {
  pinMode(ledPin, OUTPUT);      // LED como salida
  pinMode(buttonPin, INPUT_PULLUP);    // Botón como entrada
  Serial.begin(9600); //COMUNICACION SERIAL
}

void loop() {
  // Leemos el estado del botón
  buttonState = digitalRead(buttonPin);

  // Si el botón está presionado (HIGH)
  if (buttonState == LOW) {
    digitalWrite(ledPin, HIGH); // Encender LED
    Serial.println("High");
  } else {
    digitalWrite(ledPin, LOW);  // Apagar LED
  }
}

