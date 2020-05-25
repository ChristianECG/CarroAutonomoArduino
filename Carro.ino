/*
 * Carro de conducción autónoma mediante shell controlador
 * de motores y uso del sensor ultrasónico.
 * 
 * Por: ITIC <http://itsoeh.edu.mx>
 * Este código contiene una licencia de uso.
 * 
 * Modificado por última vez: 16 de abril de 2020.
 * Por: Christian Elías Cruz González
 * 
 * Historial de modificaciones:
 * 25/03/2020 - La velocidad de los motores pasa de 150 a 200
 * 27/03/2020 - Se cambió la forma en que "gira" el modelo
 * 30/03/2020 - Todos los pines pasan a un modelo de #define
 * 14/04/2020 - Se añadieron comandos de transmisión de datos por Serial
 * 15/04/2020 - Se modificaron los comentarios para todas las funciones
 * 16/04/2020 - Se cambió la velocidad de los motores, de 200 a 80
 */

// Librerías externas que se requieren para el funcionamiento del arduino
#include <Servo.h>                    // Librería que opera el Servomotor
#include <AFMotor.h>                  // Librería que opera el Shell controlador de motores

// Asignación de pines "inmutables" durante la ejecución del sistema
#define SERVO             10          // Pin asignado al Servomotor
#define Echo               9          // Pin asignado al Echo    del Ultrasónico
#define Trigger            2          // Pin asignado al Trigger del Ultrasónico
#define LedDerechaRojo    A0          // Pin asignado a uno de los led
#define LedDerechaVerde   A1          // Pin asignado a uno de los led
#define LedIzquierdaRojo  A2          // Pin asignado a uno de los led
#define LedIzquierdaVerde A3          // Pin asignado a uno de los led
#define LedFrenteRojo     A4          // Pin asignado a uno de los led
#define LedFrenteVerde    A5          // Pin asignado a uno de los led

//Declaración y asignación de servomotor y motores de corriente directa
Servo servo;                          // Declaración del servomotor
AF_DCMotor motorIzqFren(1);           // Asignación del motor (1) como motorIzqFren
AF_DCMotor motorDerFren(2);           // Asignación del motor (2) como motorDerFren
AF_DCMotor motorDerAtr(3);            // Asignación del motor (3) como motorDerAtr
AF_DCMotor motorIzqAtr(4);            // Asignación del motor (4) como motorIzqAtr

/*
 * Función de configuración inicial del programa que se ejecuta sobre la placa Arduino
 * Esta función se ejecuta UNA sola vez al iniciar el programa y no vuelve a ejecutarse
 * La función define los parámetros por defecto de nuestro servomotor, de los motores de
 * corriente directa, asegura la configuración de los pines del sensor ultrasónico y de
 * los seis leds conectados al arduino. Igualmente, asigna una velocidad de transmisión
 * al serial del arduino UNO al que se encuentra conectado el dispositivo.
 * 
 * Esta función no retorna ningún valor ni requiere parámetros de funcionamiento. No se
 * debe llamar en ninguna parte del código.
 */
void setup() {
  servo.attach(SERVO);                // Se asigna el pin asignado al servomotor
  servo.write(90);                    // Se indica al servomotor que su posición inicial es en 90°

  motorIzqFren.setSpeed(200);         // Se asigna velocidad de 80 al motorIzqFren
  motorDerFren.setSpeed(200);         // Se asigna velocidad de 80 al motorDerFren
  motorDerAtr.setSpeed(200);          // Se asigna velocidad de 80 al motorDerAtr
  motorIzqAtr.setSpeed(200);          // Se asigna velocidad de 80 al motorIzqAtr

  pinMode(Trigger, OUTPUT);           // Se asigna el pin del Trigger como pin de salida
  pinMode(Echo, INPUT);               // Se asigna el pin del Echo    como pin de entrada

  Serial.begin(9600);                 // Se inicializa la comunicación serial en 9600 baudios

  pinMode(LedDerechaRojo,   OUTPUT);  // Se asigna el pin del LedDerechaRojo    como pin de salida
  pinMode(LedDerechaVerde,  OUTPUT);  // Se asigna el pin del LedDerechaVerde   como pin de salida
  pinMode(LedIzquierdaRojo, OUTPUT);  // Se asigna el pin del LedIzquierdaRojo  como pin de salida
  pinMode(LedIzquierdaVerde,OUTPUT);  // Se asigna el pin del LedIzquierdaVerde como pin de salida
  pinMode(LedFrenteRojo,    OUTPUT);  // Se asigna el pin del LedFrenteRojo     como pin de salida
  pinMode(LedFrenteVerde,   OUTPUT);  // Se asigna el pin del LedFrenteVerde    como pin de salida
}

/*
 * Esta función es inmutable, se ejecuta constantemente durante el funcionamiento del programa,
 * hasta su finalización. Su motivo de existir es la llamada constante a la función mainCar, que
 * controla la ejecución de nuestro programa por cada iteración.
 * 
 * Esta función no retorna ningún valor ni requiere parámetros de funcionamiento. No se
 * debe llamar en ninguna parte del código.
 */
void loop() {
  mainCar();                          // Se llama a la función mainCar()
}

/*
 * Esta función controla el proceso de ejecución del carro por cada iteración del ciclo loop.
 * Obtiene constantemente la distancia que recibe del sensor ultrasónico y realiza una comparación
 * para garantizar que el espacio no se reduce. En el momento necesario, detiene todos los procesos
 * mediante una función externa, obtiene nuevos parámetros de izquierda y derecha, y compara para
 * obtener la mejor dirección.
 * 
 * Toda la información generada se envía por comunicación serial a la consola del IDE del Arduino.
 * 
 * Esta función no retorna ningún valor ni requiere parámetros de funcionamiento.
 */
void mainCar(){
  Serial.println(getDistancia());       // Obtiene la distancia y la envía por comunicación serial
  if(getDistancia()<15){                // Realiza una comparación entre la distancia que se obtiene y el número 15.

    // Las siguientes lineas de código sirven como preámbulo
    stopCar();                          // Detiene el carro
    delay(100);                         // Espera 100 ms.
    backCar();                          // Retrocede el carro
    delay(300);                         // Permanece en el estado de retroceso por 300 ms.
    stopCar();                          // Detiene el carro

    // Las siguientes lineas de código sirven para obtener información sobre lo que el carro encuentra a la izquierda
    servo.write(180);                   // El servomotor se posiciona en 180° (A la izquierda)
    delay(1000);                        // Espera 1s para que el servomotor se posicione correctamente
    int left = getDistancia();          // Obtiene la distancia que se encuentra a la izquierda, y lo almacena en una variable int
    Serial.print(left);                 // Envia el valor de la distancia por comunicación serial 
    
    Serial.print(" | ");                // Envía un símbolo por comunicación serial que servirá de división

    // Las siguientes lineas de código sirven para obtener información sobre lo que el carro encuentra a la derecha
    servo.write(0);                     // El servomotor se posiciona en   0° (A la derecha)
    delay(1000);                        // Espera 1s para que el servomotor se posicione correctamente
    int right = getDistancia();         // Obtiene la distancia que se encuentra a la derecha, y lo almacena en una variable int
    Serial.println(right);              // Envia el valor de la distancia por comunicación serial 
    
    servo.write(90);                    // El servomotor se posiciona en  90° (Al centro)

    // Las siguientes lineas de código sirven para la toma de la decisión
    if(left>right){                     // Realiza una comparación entre ambas distancias
      turnLeftCar();                    // Gira a la izquierda
      Serial.println("Left");           // Envía decisión por comunicación serial
    }
    else{
      turnRightCar();                   // Gira a la derecha
      Serial.println("Right");          // Envía decisión por comunicación serial
    }

    // Las siguientes líneas de código sirven como cierre de la transacción
    delay(800);                         // Espera 800ms para que el servomotor se posicione correctamente y para que el carro gire
    stopCar();                          // Detiene el carro
  }
  else{
    runCar();                           // Avanza de manera por defecto
  }
}

/*
 * Esta función obtiene la distancia del sensor ultrasónico mediante una sencilla operación
 * Su funcionamiento se ve implicado en cuatro partes:
 * 
 * - Primera: Envía un pulso ultrasónico (usando el pin Trigger)
 * - Segunda: Espera 10 us para recibir respuesta del pulso ultrasónico
 * - Tercera: Recibe el pulso ultrasónico
 * - Cuarta : Realiza una operación para convertir el resultado en cm.
 * 
 * @return Distancia desde el sensor ultrasónico hasta el siguiente obstáculo
 */
int getDistancia(){
  digitalWrite(Trigger, HIGH);          // Envía un pulso ultrasónico (usando el pin Trigger)
  delayMicroseconds(10);                // Espera 10 us para recibir respuesta del pulso ultrasónico
  digitalWrite(Trigger, LOW);           // Apaga el pulso ultrasónico
  return pulseIn(Echo, HIGH) / 59;      // Recibe el pulso ultrasónico y convierte el resultado en cm
}

/*
 * Esta función hace que el carro avance hacia el frente. La función consta de dos partes:
 * 
 * - Asigna la dirección que debe tomar cada uno de los motores.
 * - Asigna el voltaje que se debe enviar a cada uno de los leds.
 * 
 * Esta función no retorna ningún valor ni requiere parámetros de funcionamiento.
 */
void runCar() {
  motorIzqFren.run(BACKWARD);
  motorDerFren.run(FORWARD);
  motorDerAtr.run(FORWARD);
  motorIzqAtr.run(BACKWARD);
  
  analogWrite(LedFrenteVerde,     128);
  analogWrite(LedFrenteRojo,        0);
  analogWrite(LedDerechaVerde,      0);
  analogWrite(LedDerechaRojo,     128);
  analogWrite(LedIzquierdaVerde,    0);
  analogWrite(LedIzquierdaRojo,   128);
}

/*
 * Esta función hace que el carro gire hacia la derecha. La función consta de dos partes:
 * 
 * - Asigna la dirección que debe tomar cada uno de los motores.
 * - Asigna el voltaje que se debe enviar a cada uno de los leds.
 * 
 * Esta función no retorna ningún valor ni requiere parámetros de funcionamiento.
 */
void turnRightCar() {
  motorIzqFren.run(BACKWARD);
  motorDerFren.run(BACKWARD);
  motorDerAtr.run(BACKWARD);
  motorIzqAtr.run(BACKWARD);

  analogWrite(LedFrenteVerde,       0);
  analogWrite(LedFrenteRojo,      128);
  analogWrite(LedDerechaVerde,    128);
  analogWrite(LedDerechaRojo,       0);
  analogWrite(LedIzquierdaVerde,    0);
  analogWrite(LedIzquierdaRojo,   128);
}

/*
 * Esta función hace que el carro gire hacia la izquierda. La función consta de dos partes:
 * 
 * - Asigna la dirección que debe tomar cada uno de los motores.
 * - Asigna el voltaje que se debe enviar a cada uno de los leds.
 * 
 * Esta función no retorna ningún valor ni requiere parámetros de funcionamiento.
 */
void turnLeftCar() {
  motorIzqFren.run(FORWARD);
  motorDerFren.run(FORWARD);
  motorDerAtr.run(FORWARD);
  motorIzqAtr.run(FORWARD);

  analogWrite(LedFrenteVerde,       0);
  analogWrite(LedFrenteRojo,      128);
  analogWrite(LedDerechaVerde,      0);
  analogWrite(LedDerechaRojo,     128);
  analogWrite(LedIzquierdaVerde,  128);
  analogWrite(LedIzquierdaRojo,     0);
}

/*
 * Esta función hace que el carro detenga todos los motores. La función consta de dos partes:
 * 
 * - Asigna la dirección que debe tomar cada uno de los motores.
 * - Asigna el voltaje que se debe enviar a cada uno de los leds.
 * 
 * Esta función no retorna ningún valor ni requiere parámetros de funcionamiento.
 */
void stopCar() {
  motorIzqFren.run(RELEASE);
  motorDerFren.run(RELEASE);
  motorDerAtr.run(RELEASE);
  motorIzqAtr.run(RELEASE);

  analogWrite(LedFrenteVerde,       0);
  analogWrite(LedFrenteRojo,      128);
  analogWrite(LedDerechaVerde,      0);
  analogWrite(LedDerechaRojo,     128);
  analogWrite(LedIzquierdaVerde,    0);
  analogWrite(LedIzquierdaRojo,   128);
}

/*
 * Esta función hace que el carro retroceda. La función consta de dos partes:
 * 
 * - Asigna la dirección que debe tomar cada uno de los motores.
 * - Asigna el voltaje que se debe enviar a cada uno de los leds.
 * 
 * Esta función no retorna ningún valor ni requiere parámetros de funcionamiento.
 */
void backCar(){
  motorIzqFren.run(FORWARD);
  motorDerFren.run(BACKWARD);
  motorDerAtr.run(BACKWARD);
  motorIzqAtr.run(FORWARD);
  
  analogWrite(LedFrenteVerde,       0);
  analogWrite(LedFrenteRojo,      128);
  analogWrite(LedDerechaVerde,      0);
  analogWrite(LedDerechaRojo,     128);
  analogWrite(LedIzquierdaVerde,    0);
  analogWrite(LedIzquierdaRojo,   128);
}
