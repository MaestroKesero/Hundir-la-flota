# Hundir la flota 

# Índice

1. [Descripción Breve](#1-descripción-breve)
2. [Estado del Proyecto](#2-estado-del-proyecto)
3. [Requisitos del Sistema](#3-requisitos-del-sistema)
4. [Instrucciones de Instalación](#4-instrucciones-de-instalación)
5. [Ejemplos de Uso](#5-ejemplos-de-uso)
6. [Estructura del Proyecto](#6-estructura-del-proyecto)
7. [Contribución](#7-contribución)
8. [Notas a tener en cuenta](#8-notas-a-tener-en-cuenta)


## 1. Descripción Breve.

Programa basado en el juego de mesa "Hundir la flota" el cual está programado en C haciendo uso de estructura Cliente-Servidor usando sockets
para el intercambio de informacion, en la que varios clientes estableceran conexión al mismo mediante un login y posteriormente establecerán
 partida.


## 2. Estado del Proyecto.

El proyecto se encuentra en estado finalizado, con posibles nuevos cambios al mismo.


## 3. Requisitos del Sistema.

Los requisitos principales para ejecutar dicho programa son:

&middot; Disponer de las librerias GNU necesarias (principalmente un S.O basado en Linux)  
&middot; Pertenecer a un segmento de red debido a el acceso al servidor por parte del cliente (puede correr en loopback)  


## 4. Instrucciones de Instalación.

Para instalar dicho programa simplemente realizamos los siguientes pasos:

&middot; git clone https://github.com/MaestroKesero/Hundir-la-flota--/  
&middot; Una vez dentro de la carpeta ejecutar make para compilar el Makefile asociado  
&middot; Ejecutar el binario del servidor ./servidor   
&middot; Ejectuar el binario del cliente ./cliente <ip>  


#3 5. Ejemplos de Uso.

    Para poder jugar sin problemas a hundir la flota, deberemos de hacerlo de la siguiente forma:

&middot; Ejecutamos el 1º cliente en una terminal.
&middot; Ejecutamos el 2º cliente en una terminal.
&middot; Iniciamos sesion en el 1º cliente.
&middot; Iniciamos partida en el 1º cliente.
&middot; Iniciamos sesión en el 2º cliente.
&middot; Iniciamos partida en el 2º cliente.
&middot; ¡ A disfrutar !


## 6. Estructura del Proyecto.

El proyecto se estrucutra en 3 ficheros principales.
    
&middot; En servidor.c encontramos la logica esencial de nuestro servidor junto a los protocolos necesarios de ejecucción.
&middot; En servidor.h encontramos las funciones esenciales de hundir la flota junto a la logica necesaria de gestión del servidor.
&middot; En cliente.c encontramos la estructura esencial de nuestro cliente, junto a la implementación de errores por parte del mismo y la lógica necesaria de comunicación


## 7. Contribución.

A la hora de contribuir con el proyecto, hay un par de opciones que no estan implementadas:

&middot; La opcion de TOCADO-HUNDIDO no esta debidamente implmentada por problemas logicos.
&middot; La opción de SALIR si un cliente esta esperando partida no funciona debido a la escucha del Socket.
&middot; Faltaria encriptar los datos (si utilizamos un fichero en texto plano) o asociar una base de datos de usuarios, contraseñas, partida ganadas...
&middot; Añadirle un .log por parte del servidor.
&middot; Añadirle una interfaz gráfica.


## 8. Notas a tener en cuenta.

&middot; Si a la hora de emparejar una partida,los barcos no se generan correctamente, reiniciar el cliente.
&middot; Si un cliente falla el inicio de sesión, debera reiniciar el cliente.
