#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

void menu_ppal(void);

int main(void) {
	int opcion;
	setlocale(LC_ALL, "es-ES");
	do {
		menu_ppal();
		scanf_s("%d", &opcion);
	} while (opcion != 6);
}

void menu_ppal(void) {
	printf("Revisi�n del coche\n");
	printf("==================\n");
	printf("1. Escanee la tarjeta\n");
	printf("2. Control de alcoholemia\n");
	printf("3. Temperatura del motor\n");
	printf("4. Luminosidad\n");
	printf("5. Peso del coche\n");
	printf("6. Terminar revisi�n\n");
	printf("Introduzca una opci�n\n");
}