// Selección de modo de conexión e incluir la librería para RemoteXY
#define REMOTEXY_MODE__ESP32CORE_BLE

#include <BLEDevice.h>

// Ajuste de conexión para RemoteXY (Nombre del dispositivo para el Bluetooth)
#define REMOTEXY_BLUETOOTH_NAME "Robótica E1 G512"
#define REMOTEXY_ACCESS_PASSWORD "1512"

#include <RemoteXY.h>

// Configuración GUI para RemoteXY
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] = // 110 bytes (880 bits)
    {255, 3, 0, 0, 0, 110, 0, 19, 0, 0, 0, 82, 111, 98, 111, 116, 195, 173, 110, 0,
     25, 2, 106, 200, 200, 80, 1, 1, 5, 0, 1, 44, 53, 60, 60, 86, 22, 32, 29, 1,
     133, 31, 83, 72, 79, 79, 84, 0, 4, 13, 25, 7, 140, 159, 10, 12, 69, 48, 24, 31,
     129, 64, 68, 73, 30, 140, 3, 46, 7, 64, 16, 82, 73, 71, 72, 84, 32, 77, 79, 84,
     79, 82, 0, 129, 26, 12, 118, 18, 16, 3, 42, 7, 64, 16, 76, 69, 70, 84, 32, 77,
     79, 84, 79, 82, 0, 4, 84, 28, 8, 170, 31, 10, 12, 69, 48, 24, 31};

// Esta estructura sirve para definir nuestras variables y eventos en la interfaz de control
struct
{
  // variables de entrada
  uint8_t BTT1;     // =1 si el botón es pulsado, de lo contrario =0
  int8_t slider_02; // de -100 hasta 100 valores enteros
  int8_t slider_01; // de -100 hasta 100 valores enteros

  // Otra variable
  uint8_t connect_flag; // =1 si está conectado, de lo contrario =0
} RemoteXY;
#pragma pack(pop)
// Fin de la construcción de RemoteXY e inicio del código base

// Librería para usar la programación de Arduino NO REMOVER
#include <Arduino.h>
// Librería para controlar un servomotor con ESP32
#include <ESP32Servo.h>

/* Varibles de cableado entre la ESP32 y el puente H (L2932N)
Formato: [Pin del L2932N] = [Pin del ESP32]*/

const int ENABLE_A = 25;
const int ENABLE_B = 26;

const int IN1 = 27;
const int IN2 = 14;
const int IN3 = 12;
const int IN4 = 13;
// Nombre de la clase establecida para el servomotor
Servo PATADA;
const int servo_PATADA = 33;

// Función para controlar el ángulo del servo respecto a un botón
void SERVO(int BOTON)
{
  RemoteXY_delay(200);
  if(BOTON == 0){
    PATADA.write(0);
  }
  else{
    PATADA.write(179);
  }
}

// Función para controlar el motor respecto a cuatro variables
void MOTOR(int SLIDER, int PIN1, int PIN2, int ENABLE)
{
  // Apagar motor
  RemoteXY_delay(100);
  if (SLIDER == 0)
  {
    digitalWrite(PIN1, LOW);
    digitalWrite(PIN2, LOW);
    analogWrite(ENABLE, 0);
  }
  else
  {
    // Obtener el PWM respecto al slider
    int ABS_SLIDER = abs(SLIDER);
    int PWM = map(ABS_SLIDER, -100, 100, 0, 255);
    // Decición para la dirección de giro del motor
    if (SLIDER > 0)
    {
      // Sentido horario
      digitalWrite(PIN1, HIGH);
      digitalWrite(PIN2, LOW);
    }
    else{
      // Sentido antihorario
      digitalWrite(PIN1, LOW);
      digitalWrite(PIN2, HIGH);
    }
    RemoteXY_delay(100);
    analogWrite(ENABLE, PWM);
  }
}

void setup()
{
  // Inicialización de RemoteXY
  RemoteXY_Init();
  pinMode(ENABLE_A, OUTPUT);
  digitalWrite(ENABLE_A, LOW);

  pinMode(ENABLE_B, OUTPUT);
  digitalWrite(ENABLE_B, LOW);

  pinMode(IN1, OUTPUT);
  digitalWrite(IN1, LOW);

  pinMode(IN2, OUTPUT);
  digitalWrite(IN2, LOW);

  pinMode(IN3, OUTPUT);
  digitalWrite(IN3, LOW);

  pinMode(IN4, OUTPUT);
  digitalWrite(IN4, LOW);

  // Configuración del pin para el servomotor
  PATADA.attach(servo_PATADA);
  PATADA.write(0);
}

void loop()
{
  // Gestión de RemoteXY
  RemoteXY_Handler();

  // Control de motor izquierdo (motor A)
  MOTOR(RemoteXY.slider_01, IN2, IN1, ENABLE_A);

  // Control de motor derecho (motor B)
  MOTOR(RemoteXY.slider_02, IN4, IN3, ENABLE_B);

  // Control del servomotor PATADA
  SERVO(RemoteXY.BTT1);

}