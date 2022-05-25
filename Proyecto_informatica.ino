#include <SPI.h>
#include <MFRC522.h> // biblioteca del RFID (lector de tarjeta)
#include <Servo.h>
#include<Wire.h>
#include <Adafruit_MLX90614.h> // biblioteca del MLX90614 (sensor de temperatura)

// PINES DE LECTOR DE TARJETA
#define RST_PIN  9      // constante para referenciar pin de reset
#define SS_PIN  10      // constante para referenciar pin de slave select

// PIN DE SENSOR DE TEMPERATURA
#define Pinter 2

// PIN DE SENSOR DE LUMINOSIDAD
#define FOTOPIN A0

// PINES DE SENSOR DE ULTRASONIDOS
#define Pecho 6
#define Ptrig 7

MFRC522 mfrc522(SS_PIN, RST_PIN); // crea objeto mfrc522 enviando pines de slave select y reset

Adafruit_MLX90614 mlx = Adafruit_MLX90614();  // // crea objeto Mlx90614 equivalente al sensor de temperatura

byte Lectura_vector[4]; // el vector donde se guardara la lectura del lector RFID

String mensaje_entrada; // el mensaje de entrada desde Visual
String mensaje_salida; // el mensaje de salida a Visual

// Para compararlos con el mensaje recibido de Arduino
String temp = "TEMPERATURA ";
String luz = "LUMINOSIDAD ";
String obst = "OBSTACULO ";
char cadena[11];

void setup() {
  // put your setup code here, to run once:

  // Inicio los pines
  pinMode(7, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  pinMode(Pinter, INPUT_PULLUP);

  pinMode(Pecho, INPUT);
  pinMode(Ptrig, OUTPUT);
  
  pinMode(FOTOPIN, INPUT); // lo activamos en modo entrada  sensor de luminosidad

  mlx.begin();
  
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
         mensaje_salida = ""; //borramos el mensaje enviado tras haberlo enviado
             
         }//fin if
   }//FIN PRIMER IF  y fin de la sección escaneo tarjeta
   delay(1000);

   if (Serial.available() > 0) {
      mensaje_entrada = Serial.readStringUntil('\n');
      if (mensaje_entrada.equals(temp)) {
          // Código para la temperatura

          mensaje_salida = String(mlx.readAmbientTempC()) + "ºC";   // Hay una función que te devuelve diretamente la temperatura en forma de número
          // Lo guardo en la variable mensaje_salida

          Serial.print(mensaje_salida);  // Se la envío al arduino 

          mensaje_salida = "";  // borramos el mensaje enviado tras haberlo enviado
      }

      if (mensaje_entrada.equals(luz)) {
          // Cógigo para la luminosidad
          // El valorMapeado sirve por si le quieres dar otra escala y otros límites a los valores recibidos por el sensor de luminosidad
          // Nosotros hemos trabajado con los que te da directamente el sensor
          
          int valorSensor = 0;
          // int valorMapeado = 0;
          
          valorSensor= analogRead(FOTOPIN); // guardamos el valor del sensor
          // valorMapeado=map(valorSensor, 770, 240, 0, 255); //valor maximo, valor minimo, valor minimo del nuevo rango, valor máximo del nuevo rango

          mensaje_salida = String(valorSensor);
          // mensaje_salida = String(valorMapeado);

          Serial.print(mensaje_salida);  // Se la envío al arduino

          mensaje_salida = "";  // borramos el mensaje enviado tras haberlo enviado
      }

      if (mensaje_entrada.equals(obst)) {
          // Código para el ultrasonido
          
          //Conexión HC-SR04
          long distancia;
          long duracion=distancia;
          digitalWrite(Ptrig, LOW); //apagamos el sensor por si se encuentra encendido
          delayMicroseconds(2); // le damos tiempo suficiente para que se apage
  
          digitalWrite(Ptrig, HIGH); // encendemos el sensor y manadmos la onda sonora
          delayMicroseconds(10); 
          digitalWrite(Ptrig, LOW); // lo volvemos a apagar
          duracion= pulseIn(Pecho, HIGH); // lugar donde se recibe la onda de regreso, se elige el pin echoPin ya que es el encargado de reicibir la onda ultrasónica
          distancia=duracion/2/29.1; // medición de la distancia
          
          mensaje_salida = String(distancia)+ "cm.";  // guardamos el mensaje en la variable mensaje_salida
          Serial.println(mensaje_salida); // imprime la distancia

          mensaje_salida="";  // borramos el mensaje enviado tras haberlo enviado
      }
   }
}
