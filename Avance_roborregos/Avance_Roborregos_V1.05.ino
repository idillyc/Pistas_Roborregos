#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo miServo;

// Pines de los motores 
const int IN1 = 9; 
const int IN2 = 8;
const int IN3 = 6;
const int IN4 = 7;

// Pines sensores ultrasónicos 
const int TRIG_F = 22;
const int ECHO_F = 23;
const int TRIG_I = 24;
const int ECHO_I = 25;
const int TRIG_D = 26;
const int ECHO_D = 27;

// Pines sensores infrarrojos
const int IR_IZQ = 30;
const int IR_DER = 31;

// Pines Sensor de Color TCS3200
const int pinS0 = 2;
const int pinS1 = 3;
const int pinS2 = 4;
const int pinS3 = 11;
const int pinOut = 12;

//  Servo y Botones de Pistas
const int PIN_SERVO = 5;
const int BTN_PISTA_A = 32;
const int BTN_PISTA_B = 33;

// Configuración de movimiento y umbrales
const int UMBRAL = 15;        
const int VELOCIDAD = 180;   
const int VELOCIDAD_GIRO = 150;

long distFrente, distIzquierda, distDerecha;

// Variables canales RGB
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

String lastDisplayedColor = "";
int pistaActual = 1;

// Declaración de funciones 
void cerrarGarra();
void giroIzquierda();
void giroDerecha();
void avance();
void pistaA();
void pistaB();
void retroceder(int velocidad);
void detener();
long medirUno(int pinTrig, int pinEcho);
void medirTodos();
void leerSensorColor();

void setup() {
  // Inicialización pantalla LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Selecciona Pista");

  // Configuración motores 
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Configuración ultrasónicos
  pinMode(TRIG_F, OUTPUT);
  pinMode(ECHO_F, INPUT);
  pinMode(TRIG_I, OUTPUT);
  pinMode(ECHO_I, INPUT);
  pinMode(TRIG_D, OUTPUT);
  pinMode(ECHO_D, INPUT);

  // Configuración sensores IR
  pinMode(IR_IZQ, INPUT);
  pinMode(IR_DER, INPUT);

  // Configuración Botones 
  pinMode(BTN_PISTA_A, INPUT_PULLUP);
  pinMode(BTN_PISTA_B, INPUT_PULLUP);

  // Configuración Servo
  miServo.attach(PIN_SERVO);
  miServo.write(0); 

  // Configuración Sensor de Color 
  pinMode(pinS0, OUTPUT);
  pinMode(pinS1, OUTPUT);
  pinMode(pinS2, OUTPUT);
  pinMode(pinS3, OUTPUT);
  pinMode(pinOut, INPUT);

  // Escala de frecuencia al 20%
  digitalWrite(pinS0, HIGH);
  digitalWrite(pinS1, LOW);

  Serial.begin(9600);
}

void loop() {
  // Lectura de botones para cambiar de pista
  if (digitalRead(BTN_PISTA_A) == LOW) {
    pistaActual = 1;
    delay(300);
  }
  if (digitalRead(BTN_PISTA_B) == LOW) {
    pistaActual = 2;
  
    delay(300);
  }

  if (pistaActual == 1) {
    pistaA();
  } else {
    pistaB();
  }
}

void cerrarGarra() {
  miServo.write(90); 
  delay(500);
}

void giroIzquierda() {
  digitalWrite(IN1, LOW);
  analogWrite(IN2, VELOCIDAD_GIRO);
  analogWrite(IN3, VELOCIDAD_GIRO);
  digitalWrite(IN4, LOW);
}

void giroDerecha() {
  analogWrite(IN1, VELOCIDAD_GIRO);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  analogWrite(IN4, VELOCIDAD_GIRO);
}

void avance() {
  analogWrite(IN1, VELOCIDAD);
  digitalWrite(IN2, LOW);
  analogWrite(IN3, VELOCIDAD);
  digitalWrite(IN4, LOW);
}

// Lógica de la Pista A
void pistaA() {
  medirTodos();

  Serial.print("F:"); Serial.print(distFrente);
  Serial.print(" I:"); Serial.print(distIzquierda);
  Serial.print(" D:"); Serial.println(distDerecha);

  bool hayParedFrente = (distFrente < UMBRAL);
  bool hayParedIzquierda = (distIzquierda < UMBRAL);
  bool hayParedDerecha = (distDerecha < UMBRAL);

  if (!hayParedFrente) {
    avance();
  }
  else if (!hayParedDerecha) {
    detener();
    delay(100);
    giroDerecha();
    delay(400);
    detener();
  }
  else if (!hayParedIzquierda) {
    detener();
    delay(100);
    giroIzquierda();
    delay(400);
    detener();
  }
  else {
    detener();
    delay(100);
    giroDerecha();
    delay(800);
    detener();
  }

  leerSensorColor();
}

// Pista B
void pistaB() {

    
















}




// movimiento y sensores
void retroceder(int velocidad) {
  digitalWrite(IN1, LOW);
  analogWrite(IN2, velocidad);
  digitalWrite(IN3, LOW);
  analogWrite(IN4, velocidad);
}

void detener() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void leerSensorColor() {
  // Rojo
  digitalWrite(pinS2, LOW);
  digitalWrite(pinS3, LOW);
  redFrequency = pulseIn(pinOut, LOW, 20000);

  // Verde
  digitalWrite(pinS2, HIGH);
  digitalWrite(pinS3, HIGH);
  greenFrequency = pulseIn(pinOut, LOW, 20000);

  // Azul
  digitalWrite(pinS2, LOW);
  digitalWrite(pinS3, HIGH);
  blueFrequency = pulseIn(pinOut, LOW, 20000);

  String detectedColor = "Sin identificar";

  if (blueFrequency < redFrequency && greenFrequency < redFrequency && blueFrequency < 80) {
    detectedColor = "Turquesa / Azul";
  } 
  else if (redFrequency < 70 && greenFrequency < 80 && blueFrequency > 90) {
    detectedColor = "Amarillo";
  } 
  else if (redFrequency < 60 && greenFrequency > 70 && greenFrequency < 130 && blueFrequency > 100) {
    detectedColor = "Naranja";
  } 
  else if (redFrequency < 60 && blueFrequency < 90 && greenFrequency > redFrequency) {
    detectedColor = "Rosa";
  }

  if (detectedColor != lastDisplayedColor) {
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(detectedColor);
    lastDisplayedColor = detectedColor;
  }
}

long medirUno(int pinTrig, int pinEcho) {
  digitalWrite(pinTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);

  long duracion = pulseIn(pinEcho, HIGH, 20000);
  if (duracion == 0) {
    return 999; 
  }

  return duracion * 0.034 / 2; 
}

void medirTodos() {
  distFrente = medirUno(TRIG_F, ECHO_F);
  delay(10);
  distIzquierda = medirUno(TRIG_I, ECHO_I); 
  delay(10);
  distDerecha = medirUno(TRIG_D, ECHO_D);
}