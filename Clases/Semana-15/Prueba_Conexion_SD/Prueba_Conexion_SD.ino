#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;  // Usa el pin 4 para CS

void setup() {
  Serial.begin(9600);
  pinMode(53, OUTPUT); // Necesario en el Mega
  Serial.println("Iniciando prueba de tarjeta SD...");

  if (!SD.begin(chipSelect)) {
    Serial.println("⚠️ Error: no se pudo inicializar la tarjeta SD");
    while (1);
  }

  Serial.println("✅ Tarjeta SD inicializada correctamente.");

  File testFile = SD.open("test.txt", FILE_WRITE);
  if (testFile) {
    testFile.println("Prueba exitosa de escritura en SD.");
    testFile.close();
    Serial.println("✅ Archivo escrito correctamente.");
  } else {
    Serial.println("⚠️ No se pudo abrir el archivo test.txt");
  }
}

void loop() {}
