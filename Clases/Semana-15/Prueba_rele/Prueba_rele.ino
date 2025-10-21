// --- Configuración del relé ---
int relePin = 2;   // Pin digital conectado al IN del módulo relé

void setup() {
  Serial.begin(9600);        // Inicializa la comunicación serial
  pinMode(relePin, OUTPUT);  // Configura el pin del relé como salida
  digitalWrite(relePin, LOW); // Asegura que el relé inicie apagado
  Serial.println("Sistema iniciado. Relé listo para controlar la lámpara.");
}

void loop() {
  // --- Encender el relé ---
  digitalWrite(relePin, HIGH);  // Activa el relé (enciende la lámpara)
  Serial.println("💡 Lámpara ENCENDIDA");
  delay(10000);                 // Mantiene la lámpara encendida por 10 segundos

  // --- Apagar el relé ---
  digitalWrite(relePin, LOW);   // Desactiva el relé (apaga la lámpara)
  Serial.println("💤 Lámpara APAGADA");
  delay(10000);                 // Mantiene la lámpara apagada por 10 segundos
}
