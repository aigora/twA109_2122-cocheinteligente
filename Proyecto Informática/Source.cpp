#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "SerialClass/SerialClass.h"

#define MAX_BUFFER 200 
#define PAUSA_MS 200
#define TAM_TARJETA 4


void menu_ppal();
void leer_escaneo_tarjeta(Serial* Arduino);

char BufferSalida[MAX_BUFFER]; // Mensaje enviado a Arduino
char BufferEntrada[MAX_BUFFER]; // Mensaje recibido de Arduino

char Nico[] = "0A 38 56 19"; // C�digo de la tarjeta blanca
char Marcos[] = "C7 85 FB 32"; // C�digo del llavero
char Javi[] = "04 43 36 92"; // C�digo de la tarjeta del metro

int main(void) {
	setlocale(LC_ALL, "es-ES"); // Para los textos

	Serial* Arduino; // Declaro una clase de tipo Serial llamada Arduino
	char puerto[] = "COM7"; // Arduino est� en el puerto COM7

	char UID[200]; // Variable donde se guardar� el c�digo de la tarjeta le�da
	int cortar = 1;

	Arduino = new Serial((char*)puerto);

	if (Arduino->IsConnected()) {
		while (Arduino->IsConnected()&&cortar==1) {
			Sleep(PAUSA_MS);
			int opcion;
			do {
				menu_ppal(); // Llamo a la funci�n para imprimir el men� principal
				scanf_s("%d", &opcion); // Pido opci�n al usuario

				switch (opcion) {
				case 1:
					printf("ESCANEO DE TARJETA\n");
					leer_escaneo_tarjeta(Arduino); // Llamo a la funci�n que se encarga del escaneo de la tarjeta
					break;
				case 2:
					printf("CONTROL DE ALCOHOLEMIA\n");
					break;
				case 3:
					printf("TEMPERATURA DEL MOTOR\n");
					break;
				case 4:
					printf("LUMINOSIDAD\n");
					break;
				case 5:
					printf("OBST�CULOS\n");
					break;
				case 6:
					cortar = 0;
					break;
				default:
					printf("Opci�n incorrecta\n");
				}
			} while (opcion != 6);
		}
	}
	else { // Si Arduino no est� conectado
		printf("\nNo se ha podido conectar con Arduino.\n");
		printf("Revise la conexi�n, el puerto %s y desactive el monitor serie del IDE de Arduino.\n", puerto);
	}
}

void menu_ppal(void) {
	int opcion;
	printf("Revisi�n del coche\n");
	printf("==================\n");
	printf("1. Escanee la tarjeta\n");
	printf("2. Control de alcoholemia\n");
	printf("3. Temperatura del motor\n");
	printf("4. Luminosidad\n");
	printf("5. Obst�culos\n");
	printf("6. Terminar revisi�n\n");
	printf("Introduzca una opci�n\n");
	return;
}

void leer_escaneo_tarjeta(Serial* Arduino) {
	char UID[MAX_BUFFER];
	int bytesRecibidos;

	bytesRecibidos = Arduino->ReadData(BufferEntrada, sizeof(char) * MAX_BUFFER - 1);
	// Mete el mensaje recibido dentro de la variable BufferEntrada
	if (bytesRecibidos <= 0) {
		printf("\nNo se ha recibido respuesta a la petici�n.\n"); // Si no se recibe ning�n byte de arduino
	}
	else {
		printf("\nLa respuesta recibida tiene %d bytes.\nRecibido=%s\n", bytesRecibidos, BufferEntrada);
		// Imprime el n�mero de bytes y el mensaje recibido de Arduino
		strcpy_s(UID, MAX_BUFFER - 1, BufferEntrada); //Lo guardamos en UID para conocer el usuario 
			//y seleccionamos el numero de caracteres de BufferEntrada que queremos meter en UID
		memset(BufferEntrada, 0, 11);
		UID[11] = '\0'; //Nos aseguramos de que el UID sea solo de un usuario, si ha detectado mas de uno los borra
		printf("El usuaruio es %s.\n", UID); // Imprime el dato final
	}
	if (strcmp(UID, Marcos) == 0)
		printf("Bienvenido Marcos\n");
	else if (strcmp(UID, Javi) == 0)
		printf("Bienvenido Javi\n");
	else if (strcmp(UID, Nico) == 0)
		printf("Bienvenido Nico\n");
	else
		printf("Usuario no encontrado\n");
}