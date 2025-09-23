# leer_humedad_serial.py
import re
import time
import statistics
import serial
import matplotlib.pyplot as plt
import pandas as pd
import csv

# === Configura tu puerto y baudios ===
SERIAL_PORT = "COM6"         # Windows: "COM3", "COM5", etc.
# En Mac:  "/dev/tty.usbmodemXXXX" o "/dev/tty.usbserialXXXX"
# En Linux: "/dev/ttyACM0" o "/dev/ttyUSB0"

BAUD_RATE   = 9600                  # Velocidad de comunicación, tiene que coincidir con Arduino
MAX_SAMPLES = 200                   # cuántas muestras van a tomar
SERIAL_TIMEOUT = 2                  # segundos antes de abandonar la lectura

# -----------------------------
#   Funciones auxiliares
# -----------------------------
def parse_int(line: str):
    """
    Extrae el primer entero que aparezca en la línea.
    Soporta 'Humedad: 57%' o '57' o 'Valor crudo: 823'.
    Devuelve None si no encuentra número.
    """
    m = re.search(r"(-?\d+)", line)
    return int(m.group(1)) if m else None


def read_serial_values(port, baud, max_samples):
    values = []

    with serial.Serial(port, baud, timeout=SERIAL_TIMEOUT) as ser:
        time.sleep(2)                # dar tiempo a que Arduino arranque
        ser.reset_input_buffer()

        print(f"Leyendo hasta {max_samples} muestras desde {port} @ {baud}...")
        while len(values) < max_samples:
            try:
                raw = ser.readline()
                if not raw:
                    continue
                line = raw.decode(errors="ignore").strip()
                val = parse_int(line)
                if val is not None:
                    values.append(val)
                    print(f"{len(values):03d}: {val}")
            except KeyboardInterrupt:
                print("\nInterrumpido por el usuario.")
                break
            except Exception as e:
                print("Error leyendo/parsing:", e)

    return values


def guardar_csv(nombre_archivo, datos):
    """
    Guarda los datos en un CSV.
    Por ahora, una sola columna: 'Humedad'.
    """
    with open(nombre_archivo, 'w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        writer.writerow(["Humedad"])   # encabezado
        for d in datos:
            writer.writerow([d])


# -----------------------------
#   Main
# -----------------------------
def main():
    data = read_serial_values(SERIAL_PORT, BAUD_RATE, MAX_SAMPLES)
    if not data:
        print("No se obtuvieron datos.")
        return

    avg = statistics.fmean(data)
    print(f"\nPromedio de {len(data)} lecturas: {avg:.2f}")

    # Guardar en CSV
    guardar_csv("humedad_sensor.csv", data)
    print("Datos guardados en 'humedad_sensor.csv'.")

    # También con Pandas
    df = pd.DataFrame({"Humedad": data})
    print(df)

    # --- Gráfica simple ---
    plt.figure()
    plt.plot(range(1, len(data)+1), data)
    plt.title(f"Humedad (n={len(data)})  |  Promedio={avg:.1f}")
    plt.xlabel("Muestra")
    plt.ylabel("Lectura (porcentaje o valor crudo)")
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    main()
