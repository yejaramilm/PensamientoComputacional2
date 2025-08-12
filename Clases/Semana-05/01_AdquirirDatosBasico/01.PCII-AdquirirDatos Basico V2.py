#   Universidad EAFIT
#   Escuela de Ciencias Aplicadas e Ingeniería
#   Ingeniería Agronómica
#   Curso:      Pensamiento conputacional II
#   Programa:   Programa de integración entre arduino y python que permite leer los
#               leer datos de arduino y visualizarlos en consola del PC
#   Por:        Luis Fernando Londoño Londoño
#------------------------------------------------------------------------------

import  serial # Libreria para obtener datos del arduino
import  time   # Librerir para manejo del tiempo y la hora del sistema
from    datetime import datetime, timedelta

puerto = 'COM3'
arduino = serial.Serial(puerto, 9600)
    
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
            print("Procesando...")
            print("------------------------")
            for i in range(cantDatos):
                print("Dato #"+str(i+1))
                print("------------------------")
                list_in_floats.clear()                   # Esta variable la debo iniciar en cada ciclo, si no sigue acumulando
                leer_datos(list_in_floats)               # Llamo la función de lectura de datos on un argumento de la lista donde guardo los datos 
                visualizar_datos(list_in_floats)         # Mostrando los datos por el monitor
            print("Fin")
            arduino.close()
            break
             
def leer_datos (list_in_floats):
    list_values = []
    arduino_data = arduino.readline()
    decoded_values = str(arduino_data[0:len(arduino_data)].decode("utf-8"))
    print(decoded_values)
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
    #pHumedadS = list_in_floats[5]
    #ph = list_in_floats[6]
    #Co2 = list_in_floats[7]
    print('Temperatura oC:'+str(TemperaturaC))
    print('Temperatura oF:'+str(TemperaturaF))
    print('% Humedad Ambiente:'+str(Humedad))
    print('Indice de calor oC:'+str(IndiceCalorC))
    print('Indice de Calor oF:'+str(IndiceCalorF))
    #print('% Humedad del Suelo:'+str(pHumedadS))
    #print('Ph:'+str(ph))
    #print('Co2:'+str(Co2))
    print("------------------------")

if __name__ == '__main__':
    mainApp()


