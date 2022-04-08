#include <SPI.h>
#include <MFRC522.h> // biblioteca del RFID
#include <Servo.h>

#define RST_PIN  9      // constante para referenciar pin de reset
#define SS_PIN  10      // constante para referenciar pin de slave select

MFRC522 mfrc522(SS_PIN, RST_PIN); // crea objeto mfrc522 enviando pines de slave select y reset

byte Lectura_vector[4]; // el vector donde se guardara la lectura del lector RFID

String mensaje_entrada; // el mensaje de entrada
String mensaje_salida; // el mensaje de salida
char cadena[11];

void setup() {
  // put your setup code here, to run once:
  pinMode(7, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(9600); // Velocidad de puerto serie 9600 bits/sg.
  while(!Serial) { ; }
  SPI.begin();        // inicializa bus SPI
  mfrc522.PCD_Init(); // inicio modulo lector
}

void loop() {
  // put your main code here, to run repeatedly:
  if (mfrc522.PICC_IsNewCardPresent()){ //sale del bucle si no hay tarjeta           
        if (mfrc522.PICC_ReadCardSerial()){ //sale del bucle si no puede leer correctamente
          for (byte i = 0; i < mfrc522.uid.size; i++) { // bucle recorre de a un byte por vez el UID 
             
             if (mfrc522.uid.uidByte[i] < 0x10){   // si el byte leido es menor a 0x10
               //Serial.print("0");       // imprime espacio en blanco y numero cero
             }
             else{ 
               //Serial.print(" ");        // imprime un espacio en blanco
             }
             
             //Guardando el ID en un vector
              Lectura_vector[i]=mfrc522.uid.uidByte[i];     // almacena en VECTOR DE BYTES el byte del ID leido  
              sprintf(cadena,"%02X",mfrc522.uid.uidByte[i]);   //guarda en una cadena 2 elementos hexadecimales del vector 
              mensaje_salida=mensaje_salida+cadena+" "; // añade a un string los dos elementos hexadecimales y un espacio (el formato establecido)
              
           }//fin for
             mensaje_salida=mensaje_salida.substring(0,11); //Borramos el último espacio para no tener problemas luego
             mfrc522.PICC_HaltA();     // detiene comunicacion con tarjeta 
             
         //Enviamos DATOS Y ESPERAMOS RESPUESTA DE VISUAL      
         Serial.print(mensaje_salida); // Envío por el puerto serie (VISUAL) el codigo de la tarjeta NFC
         mensaje_salida=""; //borramos el mensaje enviado tras haberlo enviado
             
         }//fin if
   }//FIN PRIMER IF  y fin de la sección lectura NFC
}
