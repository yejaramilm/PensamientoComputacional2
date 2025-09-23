# leer_humedad_serial.py
import re
import time
import statistics
import serial
import matplotlib.pyplot as plt
import pandas as pd

# === Configura tu puerto y baudios ===
SERIAL_PORT = "COM6"         # Windows: "COM3", "COM5", etc.
# En Mac:  "/dev/tty.usbmodemXXXX" o "/dev/tty.usbserialXXXX"
# En Linux: "/dev/ttyACM0" o "/dev/ttyUSB0"

BAUD_RATE   = 9600                  # Velocidad de comunicación, tiene que coincidir con Arduino
MAX_SAMPLES = 200                   #cuantas muestras van a tomar
SERIAL_TIMEOUT = 2                  # segundos antes de abandonar la lectura

def parse_int(line: str):             #line (cadena que viene del puerto serial, por ejemplo "Humedad %: 57")
    """
    Extrae el primer entero que aparezca en la línea.
    Soporta 'Humedad: 57%' o '57' o 'Valor crudo: 823'.
    Devuelve None si no encuentra número.
    """
    m = re.search(r"(-?\d+)", line)   #esta es la funcion de captura de datos
    return int(m.group(1)) if m else None

def read_serial_values(port, baud, max_samples):
    values = []                      #lista donde se van a guardar los valores

    # Abrir puerto con los parametros dados, recuerden que se dan arriba (lineas 10,13,15)
    with serial.Serial(port, baud, timeout=SERIAL_TIMEOUT) as ser:
        # Esperar a que el Arduino reinicie al abrir el puerto
        time.sleep(2)                #Cuando se abre el puerto, el Arduino se resetea, 2s pa' terminar de arrancar
        ser.reset_input_buffer()     #limpiar buffer de entrada

        print(f"Leyendo hasta {max_samples} muestras desde {port} @ {baud}...")
        while len(values) < max_samples:  #Repite hasta juntar max_samples lecturas
            try:                          #try/except para manejar errores y Ctrl-C
                raw = ser.readline()      # lee hasta '\n' o timeout
                if not raw:               # timeout sin datos
                    continue
                line = raw.decode(errors="ignore").strip()  #decodifica a string y quita espacios
                val = parse_int(line)                       #extrae el valor numerico de la cadena
                if val is not None:                         #si se extrajo un valor numerico
                    values.append(val)
                    print(f"{len(values):03d}: {val}")      #formato de impresion
            except KeyboardInterrupt:
                print("\nInterrumpido por el usuario.")
                break
            except Exception as e:
                print("Error leyendo/parsing:", e)

    return values                          #devuelve la lista de valores 

def main():
    data = read_serial_values(SERIAL_PORT, BAUD_RATE, MAX_SAMPLES)
    if not data:                           #si la lista esta vacia
        print("No se obtuvieron datos.")
        return

    avg = statistics.fmean(data)           #calcula el promedio de la lista
    print(f"\nPromedio de {len(data)} lecturas: {avg:.2f}")

    
    datos={'Humedad':data}
    df=pd.DataFrame(datos)
    print(df)

    # --- Guardar en TXT ---
    df.to_csv("humedad.txt", index=False, header=True)

    # --- Gráfica simple ---
    plt.figure()
    plt.plot(range(1, len(data)+1), data)
    #plt.axhline(avg, linestyle="--")
    plt.title(f"Humedad (n={len(data)})  |  Promedio={avg:.1f}")
    plt.xlabel("Muestra")
    plt.ylabel("Lectura (porcentaje o valor crudo)")
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.show()


main()