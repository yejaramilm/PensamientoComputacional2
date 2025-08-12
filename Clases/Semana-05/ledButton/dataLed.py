import serial
import time

arduino = serial.Serial(port='COM6', baudrate=9600, timeout=1)
time.sleep(2)  # Espera para que Arduino reinicie

while True:
    if arduino.in_waiting > 0:
        data = arduino.readline().decode('utf-8').strip()
        print(f"Lectura: {data}")
