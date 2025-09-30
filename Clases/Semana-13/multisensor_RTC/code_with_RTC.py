# leer_humedad_serial_v2.py
import re                               # Expresiones regulares para parsear líneas
import time                             # Dormir brevemente tras abrir el puerto
import statistics                       # Calcular promedios si se desea
import serial                           # PySerial para leer desde Arduino
import matplotlib.pyplot as plt         # Gráficas
import pandas as pd                     # Manejo tabular de los datos
import csv                              # Guardar CSV si se requiere
from datetime import datetime           # Manejar timestamp como datetime

# === Configura tu puerto y baudios ===
SERIAL_PORT = "COM6"                    # Windows: "COMx"; Mac/Linux: /dev/tty.*
BAUD_RATE = 9600                        # Debe coincidir con Serial.begin() en Arduino
MAX_SAMPLES = 200                       # Número de muestras a capturar antes de cerrar
SERIAL_TIMEOUT = 2                      # Segundos de espera por cada readline()

# -----------------------------
#   Funciones auxiliares
# -----------------------------
def parse_metrics(line: str):
    """
    Parsea una línea con formato:
    'YYYY-MM-DD HH:MM:SS , Clave1: valor1 , Clave2: valor2 ...'
    Devuelve un dict con {'ts': datetime, 'Clave1': float/int, ...}
    Si no hay timestamp válido, devuelve None.
    """
    # 1) Intentar extraer el timestamp al inicio de la línea (YYYY-MM-DD HH:MM:SS)
    m_ts = re.match(r"\s*(\d{4}-\d{2}-\d{2})\s+(\d{2}:\d{2}:\d{2})", line)
    if not m_ts:
        return None  # Si no hay timestamp, descartamos la línea

    # 2) Construir objeto datetime con la fecha y hora capturadas
    ts = datetime.strptime(f"{m_ts.group(1)} {m_ts.group(2)}", "%Y-%m-%d %H:%M:%S")

    # 3) Tomar el resto de la línea después del timestamp para buscar pares clave:valor
    rest = line[m_ts.end():]

    # 4) Buscar pares tipo "Nombre %: 1" o "Temp: 23.5" separados por comas
    #    Capturamos clave (letras/números/espacios/%/_), luego ':', luego número (int o float)
    pairs = re.findall(r"([A-Za-zÁÉÍÓÚáéíóúÜüñÑ0-9 _%]+)\s*:\s*(-?\d+(?:\.\d+)?)", rest)

    # 5) Si no hay pares, al menos devolvemos el timestamp
    data = {'ts': ts}

    # 6) Normalizamos cada clave (reemplazamos espacios por '_' y quitamos '%')
    for key, val in pairs:
        clean_key = key.strip().replace('%', '').replace(' ', '_')
        # 7) Convertimos a float si hay punto, si no a int
        data[clean_key] = float(val) if '.' in val else int(val)

    # 8) Retornar el diccionario con ts y métricas encontradas
    return data


def read_serial_values(port, baud, max_samples):
    """
    Lee del puerto serial hasta max_samples registros válidos (con timestamp).
    Devuelve una lista de dicts homogéneos y el conjunto de claves métricas vistas.
    """
    rows = []                            # Acumulará dicts {'ts':..., 'Humedad':..., 'Temp':...}
    seen_keys = set()                    # Conjunto de nombres de métricas para columnas

    # 1) Abrir el puerto serial con contexto 'with' para cerrarlo automáticamente
    with serial.Serial(port, baud, timeout=SERIAL_TIMEOUT) as ser:
        time.sleep(2)                    # 2) Dar tiempo a que Arduino reinicie al abrir el puerto
        ser.reset_input_buffer()         # 3) Vaciar buffer de entrada para empezar "limpio"

        print(f"Leyendo hasta {max_samples} muestras desde {port} @ {baud}...")
        # 4) Leer línea a línea hasta acumular max_samples dicts válidos
        while len(rows) < max_samples:
            try:
                raw = ser.readline()     # 5) Leer una línea (termina en '\n' o por timeout)
                if not raw:
                    continue             # 6) Si no llegó nada, intentar de nuevo

                line = raw.decode(errors="ignore").strip()  # 7) Decodificar bytes → str y limpiar
                data = parse_metrics(line)                   # 8) Intentar parsear timestamp + métricas

                if data is None:
                    # 9) Si no parsea (línea parcial o ruido), la ignoramos
                    continue

                rows.append(data)        # 10) Guardar el dict válido
                seen_keys.update(k for k in data.keys() if k != 'ts')  # 11) Registrar métricas vistas
                print(f"{len(rows):03d}: {data}")            # 12) Eco de lo leído para depuración

            except KeyboardInterrupt:
                print("\nInterrumpido por el usuario.")
                break
            except Exception as e:
                print("Error leyendo/parsing:", e)

    # 13) Devolver la lista de registros y las claves de métricas
    return rows, sorted(seen_keys)


def guardar_csv(nombre_archivo, df: pd.DataFrame):
    """
    Guarda el DataFrame completo (incluye 'ts' y todas las métricas) en CSV.
    """
    df.to_csv(nombre_archivo, index=False, encoding='utf-8')


# -----------------------------
#   Main
# -----------------------------
def main():
    # 1) Leer registros crudos y las claves de métricas detectadas
    rows, metric_keys = read_serial_values(SERIAL_PORT, BAUD_RATE, MAX_SAMPLES)

    # 2) Validar que obtuvimos datos
    if not rows:
        print("No se obtuvieron datos.")
        return

    # 3) Convertir a DataFrame para manejo y graficación
    df = pd.DataFrame(rows)

    # 4) Asegurar que 'ts' sea datetime (por si en algún sistema viene como str)
    df['ts'] = pd.to_datetime(df['ts'])

    # 5) Ordenar por tiempo por si el buffer llegó desordenado
    df.sort_values('ts', inplace=True)

    # 6) Rellenar NaN si hubo sensores que no reportaron en alguna línea
    df.fillna(method='ffill', inplace=True)   # 6a) Propagar último valor conocido hacia adelante
    df.fillna(method='bfill', inplace=True)   # 6b) Y hacia atrás por si el primero fue NaN

    # 7) Imprimir resumen simple por cada métrica
    for k in metric_keys:
        avg = df[k].mean()
        print(f"Promedio de {k} en {len(df)} lecturas: {avg:.2f}")

    # 8) Guardar CSV con timestamp y todas las columnas de sensores
    guardar_csv("sensores_timestamp.csv", df)
    print("Datos guardados en 'sensores_timestamp.csv'.")

    # 9) Graficar: una curva por métrica sobre el tiempo
    plt.figure()                           # 9a) Crear figura
    for k in metric_keys:
        plt.plot(df['ts'], df[k], label=k) # 9b) Dibujar cada serie k contra el tiempo
    plt.title("Lecturas de sensores vs tiempo")
    plt.xlabel("Tiempo")
    plt.ylabel("Valor")
    plt.grid(True, alpha=0.3)
    plt.legend()                           # 9c) Mostrar leyenda con nombres de sensores
    plt.tight_layout()
    plt.show()

    # 10) (Opcional) Si quieres mantener el estilo antiguo por índice de muestra:
    # plt.figure()
    # for k in metric_keys:
    #     plt.plot(range(1, len(df)+1), df[k], label=k)
    # plt.title("Lecturas de sensores (por índice)")
    # plt.xlabel("Muestra")
    # plt.ylabel("Valor")
    # plt.grid(True, alpha=0.3)
    # plt.legend()
    # plt.tight_layout()
    # plt.show()


if __name__ == "__main__":
    main()
