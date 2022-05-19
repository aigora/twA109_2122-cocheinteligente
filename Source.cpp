#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "SerialClass/SerialClass.h"

#define TAM_TEXTO 50
#define MAX_BUFFER 200 
#define PAUSA_MS 200
#define TAM_TARJETA 12
#define MAX_USUARIOS 10


char BufferSalida[MAX_BUFFER]; // Mensaje enviado a Arduino
char BufferEntrada[MAX_BUFFER]; // Mensaje recibido de Arduino

int bytesRecibidos;

typedef struct {
	char codigo_tarjeta[TAM_TARJETA];
	char nombre[TAM_TEXTO];
	int telefono;
	int edad;
	int novel;
} Usuario;


void menu_ppal();
Usuario leer_escaneo_tarjeta(Serial* Arduino);
void crear_fichero_dat(Usuario a[], int n);
int leer_fichero_dat(Usuario a[]);
void control_alcoholemia(Usuario aux);
void temperatura(Serial* Arduino);
void luminosidad(Serial* Arduino);
void obstaculo(Serial* Arduino);
float float_from_cadena(char a[]);


Usuario usuarios[MAX_USUARIOS] = { {"0A 38 56 19", "Nicolás", 611412522, 18, 1},
{"04 43 36 92", "Javier", 639107560, 18, 0},
{"C7 85 FB 32", "Marcos", 616551911, 19, 1} };
int nusuarios = 3;

char buffer_intro;


int main(void) {

	setlocale(LC_ALL, "es-ES"); // Para los textos

	Serial* Arduino; // Declaro una clase de tipo Serial llamada Arduino
	char puerto[] = "COM7"; // Arduino está en el puerto COM7

	char UID[200]; // Variable donde se guardará el código de la tarjeta leída
	int cortar = 1;
	int hay_usuario = 0;

	Arduino = new Serial((char*)puerto);

	// nusuarios = leer_fichero_dat(usuarios);

	Usuario conductor;

	if (Arduino->IsConnected()) {
		while (Arduino->IsConnected()&&cortar==1) {
			Sleep(PAUSA_MS);
			int opcion;
			do {
				menu_ppal(); // Llamo a la función para imprimir el menú principal
				scanf_s("%d", &opcion); // Pido opción al usuario

				if (hay_usuario == 0 && opcion != 1)
					printf("Antes de poder revisar las condiciones -> Escanee tarjeta\n");
				// Para poder arrancar el coche y el sistema hay que escanear una tarjeta (tiene que haber un usuario)

				switch (opcion) {
				case 1:
					printf("ESCANEO DE TARJETA\n");
					conductor = leer_escaneo_tarjeta(Arduino); // Llamo a la función que se encarga del escaneo de la tarjeta
					// Y recibo el usuario cuya tarjeta se haya escaneado
					hay_usuario = 1;	// Alguien ha "metido la llave en el contacto" (ha escaneado la tarjeta)
					break;
				case 2:
					printf("CONTROL DE ALCOHOLEMIA\n");
					control_alcoholemia(conductor);
					break;
				case 3:
					printf("TEMPERATURA DEL MOTOR\n");
					temperatura(Arduino);
					break;
				case 4:
					printf("LUMINOSIDAD\n");
					luminosidad(Arduino);
					break;
				case 5:
					printf("OBSTÁCULOS\n");
					obstaculo(Arduino);
					break;
				case 6:
					cortar = 0;
					break;
				default:
					printf("Opción incorrecta\n");
				}
			} while (opcion != 6);
			crear_fichero_dat(usuarios, nusuarios);
		}
	}
	else { // Si Arduino no está conectado
		printf("\nNo se ha podido conectar con Arduino.\n");
		printf("Revise la conexión, el puerto %s y desactive el monitor serie del IDE de Arduino.\n", puerto);
	}
}

void menu_ppal(void) {
	int opcion;
	printf("Revisión del coche\n");
	printf("==================\n");
	printf("1. Escanee la tarjeta\n");
	printf("2. Control de alcoholemia\n");
	printf("3. Temperatura del motor\n");
	printf("4. Luminosidad\n");
	printf("5. Obstáculos\n");
	printf("6. Terminar revisión\n");
	printf("Introduzca una opción\n");
	return;
}

Usuario leer_escaneo_tarjeta(Serial* Arduino) {
	char UID[MAX_BUFFER];
	int i, existente = 0;
	Usuario aux = {};

	bytesRecibidos = Arduino->ReadData(BufferEntrada, sizeof(char) * MAX_BUFFER - 1);
	// Mete el mensaje recibido dentro de la variable BufferEntrada
	if (bytesRecibidos <= 0) {
		printf("\nNo se ha recibido respuesta a la petición.\n"); // Si no se recibe ningún byte de Arduino
	}
	else {
		printf("\nLa respuesta recibida tiene %d bytes.\nRecibido = %s\n", bytesRecibidos, BufferEntrada);
		// Imprime el número de bytes y el mensaje recibido de Arduino
		strcpy_s(UID, MAX_BUFFER - 1, BufferEntrada); // Lo guardamos en UID para conocer el usuario 
			// y seleccionamos el numero de caracteres de BufferEntrada que queremos meter en UID
		memset(BufferEntrada, 0, 11);	// Borro los datos de los primeros 11 carácteres para la próxima función
		UID[12] = '\0'; // Nos aseguramos de que el UID sea solo de un usuario, si ha detectado mas de uno los borra
		printf("El usuaruio es %s.\n", UID); // Imprime el dato final
	}
	for (i = 0; i < nusuarios; i++) {
		if (strcmp(UID, usuarios[i].codigo_tarjeta) == 0) {		// Lo comparo con los usarios existentes
			printf("Bienvenido %s\n", usuarios[i].nombre);		// Le doy la bienvenida con su nombre
			aux = usuarios[i];									// Devuelvo ese usuario
			existente = 1;										// Es un usuario existente
		}
	}

	if (existente == 0) {							// No existe el usuario encontrado
		printf("Usuario no encontrado\n");			// Creamos un nuevo usuario con la tarjeta que se ha escaneado
		char cod_tarjeta[10];						// Le pedimos los datos por teclado
		strcpy_s(usuarios[nusuarios].codigo_tarjeta, MAX_BUFFER - 1, UID);
		printf("Introduzca su nombre: ");
		scanf_s("%s", usuarios[nusuarios].nombre, TAM_TEXTO);
		scanf_s("%c", &buffer_intro);				// Para quitar el intro de buffer de teclado
		printf("Introduzca su teléfono: ");
		scanf_s("%d", &usuarios[nusuarios].telefono);
		printf("Introduzca su edad: ");
		scanf_s("%d", &usuarios[nusuarios].edad);
		printf("Introduzca 1 si es nóvel y 0 si no lo es: ");
		scanf_s("%d", &usuarios[nusuarios].novel);
		nusuarios++;								// Aumento el número de usuarios
		return usuarios[nusuarios];
	}
	
	bytesRecibidos = -1;	// Le pongo este valor para la próxima vez que reciba bytes de Arduino
	return aux;		// Le devuelvo el usuario escaneado
}

void control_alcoholemia(Usuario aux) {
	printf("Introduzca su tasa de alcoholemia:\n");
	float tasa;
	scanf_s("%f", &tasa);	// Para que el usuario introduzca su tasa de alcoholemia
	if (aux.novel == 0) {	// Si no es novel su tasa de alcohol permitida es más grande
		if (tasa > 0.25)
			printf("Su tasa de alcoholemia supera la permitida.\n");
		else
			printf("Tasa de alcoholemia está dentro de los límites.\n");
	}
	else {					// Si es novel la tasa de alcohol permitida es más baja
		if (tasa > 0.15)
			printf("Su tasa de alcoholemia supera la permitida.\n");
		else
			printf("Tasa de alcoholemia está dentro de los límites.\n");
	}
}

void temperatura(Serial* Arduino) {
	char tempc[MAX_BUFFER];
	float tempn;

	strcpy_s(BufferSalida, "TEMPERATURA \n");	// Introduzco el texto en BufferEntrada para enviarselos a Arduino
	Arduino->WriteData(BufferSalida, strlen(BufferSalida));		// Le envío el BufferEntrada a arduino
	Sleep(PAUSA_MS);	// Le doy timepo al Arduino tiempo para realizar los cálculos
	Sleep(PAUSA_MS);	// Le doy tiempo al Arduino aún más tiempo para realizar los cálculos (por si acaso)

	bytesRecibidos = Arduino->ReadData(BufferEntrada, sizeof(char) * MAX_BUFFER - 1);
	// Mete el mensaje recibido dentro de la variable BufferEntrada

	if (bytesRecibidos <= 0) {
		printf("\nNo se ha recibido respuesta a la petición.\n"); // Si no se recibe ningún byte de Arduino
	}
	else {
		printf("\nLa respuesta recibida tiene %d bytes.\nRecibido = %s\n", bytesRecibidos, BufferEntrada);
		// Imprime el número de bytes y el mensaje recibido de Arduino
		strcpy_s(tempc, MAX_BUFFER - 1, BufferEntrada); // Lo guardamos en tempc para sacar el número de la cadena 
			// y seleccionamos el numero de caracteres de BufferEntrada que queremos meter en tempc
		memset(BufferEntrada, 0, 10);	// Borro los datos de los primeros 10 carácteres para la próxima función
		// tempc[3] = '\0'; //Nos aseguramos de que el UID sea solo de un usuario, si ha detectado mas de uno los borra
		printf("La temperatura del motor es %s.\n", tempc); // Imprime el dato final
	}

	tempn = float_from_cadena(tempc);	// Saco el número de la cadena recibida y lo meto en la variable
	
	if (tempn > 5) {
		printf("La temperatura del motor es óptima. Puede arrancar\n");
	}
	else {				// Si la temperatura está por debajo de 5ºC se considera demasiado frío
		printf("El motor está demasiado frio\n");
		printf("Deje el motor en marcha para que entre en calor\n");
	}
	bytesRecibidos = -1;	// Le pongo este valor para la próxima vez que reciba bytes de Arduino
	memset(BufferSalida, 0, 20);	// Pongo un valor nulo para los primeros 20 caracteres para la próxima función
}

void luminosidad(Serial* Arduino) {
	char lumc[MAX_BUFFER];
	float lumn;

	strcpy_s(BufferSalida, "LUMINOSIDAD \n");
	Arduino->WriteData(BufferSalida, strlen(BufferSalida));		// Le envío el BufferEntrada a arduino
	Sleep(PAUSA_MS);	// Le doy timepo al Arduino tiempo para realizar los cálculos
	Sleep(PAUSA_MS);	// Le doy tiempo al Arduino aún más tiempo para realizar los cálculos (por si acaso)

	bytesRecibidos = Arduino->ReadData(BufferEntrada, sizeof(char) * MAX_BUFFER - 1);
	// Mete el mensaje recibido dentro de la variable BufferEntrada

	if (bytesRecibidos <= 0) {
		printf("\nNo se ha recibido respuesta a la petición.\n"); // Si no se recibe ningún byte de Arduino
	}
	else {
		printf("\nLa respuesta recibida tiene %d bytes.\nRecibido = %s\n", bytesRecibidos, BufferEntrada);
		// Imprime el número de bytes y el mensaje recibido de Arduino
		strcpy_s(lumc, MAX_BUFFER - 1, BufferEntrada); // Lo guardamos en distc para sacar el número de la cadena 
			//y seleccionamos el numero de caracteres de BufferEntrada que queremos meter en UID
		memset(BufferEntrada, 0, 10);	// Borro los datos de los primeros 10 carácteres para la próxima función
		// lumc[3] = '\0'; //Nos aseguramos de que el UID sea solo de un usuario, si ha detectado mas de uno los borra
		printf("La distancia es %s.\n", lumc); // Imprime el dato final
	}

	lumn = float_from_cadena(lumc);	// Saco el número de la cadena recibida y lo meto en la variable

	if (lumn > 20) {
		printf("No hay problemas con la visibilidad de la carretera\n");
	}
	else {				// Si da un valor menor a x se considera que hay poca luz
		printf("No hay suficiente luz, la visión puede estar entorpecida\n");
		printf("Debe conducir con los faros puestos, o esperar a que se haga de día\n");
	}
	
	bytesRecibidos = -1;	// Le pongo este valor para la próxima vez que reciba bytes de Arduino
	memset(BufferSalida, 0, 20);	// Pongo un valor nulo para los primeros 20 caracteres para la próxima función
}

void obstaculo(Serial* Arduino) {
	char distc[MAX_BUFFER];
	float distn;

	strcpy_s(BufferSalida, "OBSTACULO \n");		// Introduzco el texto en BufferEntrada para enviarselos a Arduino
	Arduino->WriteData(BufferSalida, strlen(BufferSalida));		// Le envío el BufferEntrada a arduino
	Sleep(PAUSA_MS);	// Le doy timepo al Arduino tiempo para realizar los cálculos
	Sleep(PAUSA_MS);	// Le doy tiempo al Arduino aún más tiempo para realizar los cálculos (por si acaso)

	bytesRecibidos = Arduino->ReadData(BufferEntrada, sizeof(char) * MAX_BUFFER - 1);
	// Mete el mensaje recibido dentro de la variable BufferEntrada

	if (bytesRecibidos <= 0) {
		printf("\nNo se ha recibido respuesta a la petición.\n"); // Si no se recibe ningún byte de Arduino
	}
	else {
		printf("\nLa respuesta recibida tiene %d bytes.\nRecibido = %s\n", bytesRecibidos, BufferEntrada);
		// Imprime el número de bytes y el mensaje recibido de Arduino
		strcpy_s(distc, MAX_BUFFER - 1, BufferEntrada); // Lo guardamos en distc para sacar el número de la cadena 
			//y seleccionamos el numero de caracteres de BufferEntrada que queremos meter en UID
		memset(BufferEntrada, 0, 10);	// Borro los datos de los primeros 10 carácteres para la próxima función
		// distc[3] = '\0'; //Nos aseguramos de que el UID sea solo de un usuario, si ha detectado mas de uno los borra
		printf("La distancia es %s.\n", distc); // Imprime el dato final
	}

	distn = float_from_cadena(distc);	// Saco el número de la cadena recibida y lo meto en la variable
	
	if (distn > 20) {
		printf("No hay obstáculos cerca\n");
	}
	else {				// Si está a menos de 20 centímetros se considera un obstáculo
		printf("Hay un obstáculo delante\n");
		printf("Dé marcha atrás o quite el obstáculo y vuelva a intentarlo\n");
	}
	bytesRecibidos = -1;	// Le pongo este valor para la próxima vez que reciba bytes de Arduino
	memset(BufferSalida, 0, 20);	// Pongo un valor nulo para los primeros 20 caracteres para la próxima función
}

float float_from_cadena(char a[]) {
	int estado = 0, i, divisor = 10;
	float numero;

	for (i = 0; i < MAX_BUFFER; i++) {
		switch (estado) {
		case 0:
			if (a[i] >= '0' && a[i] <= '9')		// Si el carácter es un número
			{
				numero = a[i] - '0';	// Lo guarda en el número
				estado = 1;				// Y se va al caso 1
			}
			break;
		case 1:
			if (a[i] >= '0' && a[i] <= '9')			// Sigue haciendo lo mismo
				numero = numero * 10 + a[i] - '0';	// Moviendo todo el número una a la izquierda (*10)
			else
				if (a[i] == '.' || a[i] == ',')		// Hasta que se encuentre una coma
					estado = 2;						// Te vas a la parte decimal
				else
					estado = 3;
			break;
		case 2: // Parte decimal
			if (a[i] >= '0' && a[i] <= '9')							// Hasta que deje de ser un número
			{
				numero = numero + (float)(a[i] - '0') / divisor;	// Voy añadiendo los decimales
				divisor *= 10;
			}
			else
				estado = 3;
			break;
		}
	}
	return numero;		// Devuelvo el número completo
}

void crear_fichero_dat(Usuario a[], int n) {
	FILE* fichero;
	int i;
	errno_t e;

	e = fopen_s(&fichero, "Usuarios.dat", "wb");	// Abro el fichero "Usuarios"

	if (fichero == NULL)	// Si no encuentra el fichero
		printf("No se ha podido guardar la agenda\n");
	else {
		fwrite(&n, sizeof(int), 1, fichero);	// Guarda el número de usuarios actual
		fwrite(a, sizeof(Usuario), n, fichero);	// Guarda los n primeros usuarios (los únicos que tendrán datos)
		fclose(fichero);		// Cierro el fichero
	}
}

int leer_fichero_dat(Usuario a[]) {
	FILE* fichero;
	int n = 0;
	errno_t e;

	e = fopen_s(&fichero, "Usuarios.dat", "rb");	// Abro el fichero "Usuarios"

	if (fichero == NULL)
		printf("La agenda estaba vacía\n");		// Si no encuentra el fichero
	else {
		fread(&n, sizeof(int), 1, fichero);		// Lee el primer valor (un entero) equivalente al número de usuarios
		fread(a, sizeof(Usuario), n, fichero);	// Lee los primeros n usuarios ya registrados
		fclose(fichero);		// Cierro el fichero
	}
	return n;
}
