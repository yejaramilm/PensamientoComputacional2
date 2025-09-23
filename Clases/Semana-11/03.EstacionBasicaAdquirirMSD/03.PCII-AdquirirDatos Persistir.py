# EAFIT - Pensamiento Computacional II
# Programa capturar datos, persistirlos y visualizarlos
# Versión 1.0
#---------------------------------------------------------------------


import serial # Libreria para obtener datos del arduino
import time   # Librerir para manejo del tiempo y la hora del sistema
from datetime import datetime, timedelta
import csv    # Libreria para el manejo de archivos tipo Excel (.csv)
import os

plot = 'COM5'
arduino = serial.Serial(plot, 9600)
    
def mainApp():
    list_in_floats = []                # Defino una variable tipo arrglo para guardar los datos que lea
    while True:
        try:
            cantDatos = int(input("Cantidad de datos a capturar: ")) #Captura la cantidad de datos que quiero capturar
        except ValueError:
            print("Error: no es un valor numerico")
        except:
            print("Error: no se ha ingresado datos")
        else:
            ftxt = open('datosPCII- nuevo.txt', 'a+')

            archivoCSV = 'datosPCII-nuevo.csv'

            if os.path.isfile(archivoCSV):
                fcsv = open(archivoCSV, 'a+')
            else:
                fcsv = open(archivoCSV, 'a+')
                headerList = ['Fecha','oC', 'oF', '%H', 'IoC', 'IoF', 'I%H'] 
                dw = csv.DictWriter(fcsv, delimiter=';',fieldnames=headerList) 
                dw.writeheader() 
                
            print("Procesando...")
            for i in range(cantDatos):
                print(".")
                list_in_floats.clear()
                leer_datos(list_in_floats)               # Llamo la función de lectura de datos on un argumento de la lista donde guardo los datos 
                visualizar_datos(list_in_floats)         # Mostrando los datos por el monitor
                guardar_datos_txt(list_in_floats, ftxt)  # Invoco función para guardar los datos en un archivo txt
                guardar_datos_csv(list_in_floats, fcsv)  # Invoco función para guardar los datos en un archivo cvs
            print("Fin")
            ftxt.close()
            fcsv.close()
            arduino.close()
            break
             
def leer_datos (list_in_floats):
    list_values = []
    arduino_data = arduino.readline()
    decoded_values = str(arduino_data[0:len(arduino_data)].decode("utf-8"))
    list_values = decoded_values.split(';')
    for item in list_values:
        list_in_floats.append(float(item))
    arduino_data = 0
    
def visualizar_datos(list_in_floats):
    TemperaturaC = list_in_floats[0]
    Humedad = list_in_floats[1]
    TemperaturaF = list_in_floats[2]
    IndiceCalorC = list_in_floats[3]
    IndiceCalorF = list_in_floats[4]
    print('Temperatura oC:'+str(TemperaturaC))
    print('Temperatura oF:'+str(TemperaturaF))
    print('Humedad:'+str(Humedad))
    print('Indice de calor oC:'+str(IndiceCalorC))
    print('Indice de Calor oF:'+str(IndiceCalorF))
    
def guardar_datos_txt(list_in_floats, f):
    tiempoActual = str(datetime.now()).format('HH-MM.SS.SSS')
    datos_txt = tiempoActual+':'+str(list_in_floats[0])+','+str(list_in_floats[1])+','+str(list_in_floats[2]) + ','+ str(list_in_floats[3])+','+str(list_in_floats[4])
    f.writelines(datos_txt+'\n')

def guardar_datos_csv(list_in_floats, f):
    tiempoActual = str(datetime.now()).format('HH-MM.SS.SSS')
    datos_csv = tiempoActual+';'+str(list_in_floats[0])+';'+str(list_in_floats[1])+';'+str(list_in_floats[2]) + ';'+ str(list_in_floats[3])+';'+str(list_in_floats[4])
    f.writelines(datos_csv+'\n')

if __name__ == '__main__':
    mainApp()



