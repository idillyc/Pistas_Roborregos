#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo miServo;

// Pines
const int IN1 = 9;
const int IN2 = 8;
const int IN3 = 7;
const int IN4 = 6;

const int TRIG_F = 22;
const int ECHO_F = 23;
const int TRIG_I = 24;
const int ECHO_I = 25;
const int TRIG_D = 26;
const int ECHO_D = 27;

const int IR_IZQ = 30;
const int IR_DER = 31;

const int pinS0 = 38;
const int pinS1 = 39;
const int pinS2 = 4;
const int pinS3 = 11;
const int pinOut = 12;

const int PIN_SERVO = 13;

const int BTNa = 33;
const int BTNb = 32;

// Constantes 
const int VELOCIDAD = 95;
const int VELOCIDAD_GIRO = 100;

const int TIEMPO_UNIDAD = 1200;
const int TIEMPO_GIRO = 550;
const int TIEMPO_GIRO_180 = 1450;

const int UMBRAL = 12;
const int UMBRAL_ENTRADA = 20;
const int DIST_PELOTA = 6;
const int DIST_MIN = 5;
const int TIEMPO_PELOTA = 700;  

const int GARRA_ABIERTA = 0;
const int GARRA_CERRADA = 90;

const int BLANCO = LOW;

const bool DEBUG = false;         

const unsigned long DEBOUNCE = 50;

// Colores
const int C_NINGUNO = 0;
const int C_CIAN = 1;
const int C_AMARILLO = 2;
const int C_NARANJA = 3;
const int C_ROSA = 4;
const int C_ROJO = 5;
const int C_VERDE = 6;

// Direcciones respecto a la pista
const int ARRIBA = 0;
const int DERECHA = 1;
const int ABAJO = 2;
const int IZQUIERDA = 3;

// Variables
int modo = 0;
unsigned long ultimoCambio = 0;

long distFrente, distIzquierda, distDerecha;

int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

int ultimoColor = -1;

int seccionB = 1;
bool iniciadoB = false;
bool tieneBola = false;
int posicionAnillo = 0;
int rumbo = ARRIBA;

int callejonesSeguidos = 0;

// Prototipos de funciones
void leerBotones();
void avance();
void retroceder(int velocidad);
void giroIzquierda();
void giroDerecha();
void detener();
void girar90Izquierda();
void girar90Derecha();
void girar180();
void avanzarTiempo(int tiempo);
void avanzarUnidad();
void avanzarMediaUnidad();
void cerrarGarra();
void abrirGarra();
long medirUno(int pinTrig, int pinEcho);
void medirTodos();
int detectarColor();
void mostrarColor();
void pistaA();
void pistaB();
void reiniciarPistaB();
void mostrarSeccion();
void buscarCheckpoint(int tiempoMaximo);
void seccion1();
void pasoAnillo();
void revisarCentro();
void seccion2();
void evadirLinea();
void seccion3();
void girarARumbo(int destino);

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Selecciona Pista");

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(TRIG_F, OUTPUT);
  pinMode(ECHO_F, INPUT);
  pinMode(TRIG_I, OUTPUT);
  pinMode(ECHO_I, INPUT);
  pinMode(TRIG_D, OUTPUT);
  pinMode(ECHO_D, INPUT);

  pinMode(IR_IZQ, INPUT);
  pinMode(IR_DER, INPUT);

  pinMode(BTNa, INPUT_PULLUP);
  pinMode(BTNb, INPUT_PULLUP);

  miServo.attach(PIN_SERVO);
  miServo.write(GARRA_ABIERTA);

  pinMode(pinS0, OUTPUT);
  pinMode(pinS1, OUTPUT);
  pinMode(pinS2, OUTPUT);
  pinMode(pinS3, OUTPUT);
  pinMode(pinOut, INPUT);
  digitalWrite(pinS0, HIGH);
  digitalWrite(pinS1, LOW);

  Serial.begin(9600);
}

void loop() {
  leerBotones();

  if (modo == 1) {
    pistaA();
  }
  else if (modo == 2) {
    pistaB();
  }
  else {
    detener();
  }
}

void leerBotones() {
  if (millis() - ultimoCambio < DEBOUNCE) {
    return;
  }

  if (digitalRead(BTNa) == LOW) {
    modo = 1;
    ultimoCambio = millis();
  }
  else if (digitalRead(BTNb) == LOW) {
    if (modo != 2) {
      reiniciarPistaB();
    }
    modo = 2;
    ultimoCambio = millis();
  }
}

void avance() {
  analogWrite(IN1, VELOCIDAD - 20);
  analogWrite(IN2, 0);
  analogWrite(IN3, VELOCIDAD);
  analogWrite(IN4, 0);
}

void retroceder(int velocidad) {
  analogWrite(IN1, 0);
  analogWrite(IN2, velocidad - 20);
  analogWrite(IN3, 0);
  analogWrite(IN4, velocidad);
}

void giroIzquierda() {
  analogWrite(IN1, 0);
  analogWrite(IN2, VELOCIDAD_GIRO - 20);
  analogWrite(IN3, VELOCIDAD_GIRO);
  analogWrite(IN4, 0);
}

void giroDerecha() {
  analogWrite(IN1, VELOCIDAD_GIRO - 20);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, VELOCIDAD_GIRO);
}

void detener() {
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
}

void girar90Izquierda() {
  detener();
  delay(100);
  giroIzquierda();
  delay(TIEMPO_GIRO);
  detener();
  delay(100);
  rumbo = (rumbo - 1 + 4) % 4; // Actualizar rumbo global
}

void girar90Derecha() {
  detener();
  delay(100);
  giroDerecha();
  delay(TIEMPO_GIRO);
  detener();
  delay(100);
  rumbo = (rumbo + 1) % 4; // Actualizar rumbo global
}

void girar180() {
  detener();
  delay(100);
  giroDerecha();
  delay(TIEMPO_GIRO_180);
  detener();
  delay(100);
  rumbo = (rumbo + 2) % 4; // Actualizar rumbo global
}

void avanzarTiempo(int tiempo) {
  avance();
  delay(tiempo);
  detener();
}

void avanzarUnidad() {
  unsigned long inicio = millis();

  while (millis() - inicio < TIEMPO_UNIDAD) {
    distFrente = medirUno(TRIG_F, ECHO_F);

    if (distFrente < DIST_MIN) {
      break;
    }
    avance();
  }

  detener();
  delay(100);
}

void avanzarMediaUnidad() {
  unsigned long inicio = millis();

  while (millis() - inicio < TIEMPO_UNIDAD / 2) {
    distFrente = medirUno(TRIG_F, ECHO_F);

    if (distFrente < DIST_MIN) {
      break;
    }
    avance();
  }

  detener();
  delay(100);
}

// Garra
void cerrarGarra() {
  miServo.write(GARRA_CERRADA);
  delay(500);
}

void abrirGarra() {
  miServo.write(GARRA_ABIERTA);
  delay(500);
}

// Ultrasonicos
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

// Color 
int detectarColor() {
  digitalWrite(pinS2, LOW);
  digitalWrite(pinS3, LOW);
  redFrequency = pulseIn(pinOut, LOW, 20000);

  digitalWrite(pinS2, HIGH);
  digitalWrite(pinS3, HIGH);
  greenFrequency = pulseIn(pinOut, LOW, 20000);

  digitalWrite(pinS2, LOW);
  digitalWrite(pinS3, HIGH);
  blueFrequency = pulseIn(pinOut, LOW, 20000);

  if (DEBUG) {
    Serial.print("R: ");
    Serial.print(redFrequency);
    Serial.print("  G: ");
    Serial.print(greenFrequency);
    Serial.print("  B: ");
    Serial.println(blueFrequency);
  }

  int r = redFrequency;
  int g = greenFrequency;
  int b = blueFrequency;

  // Lectura invalida (timeout)
  if (r == 0 || g == 0 || b == 0) {
    return C_NINGUNO;
  }

  
  if (r >= 32 && g < r - 8) {
    return C_VERDE;
  }

  else if (r >= 17 && r < 32 && g * 10 < r * 7 && b <= g) {
    return C_CIAN;
  }
  else if (r <= 14 && g <= 14 && g * 10 <= r * 17) {
    return C_AMARILLO;
  }
 
  else if (r <= 14 && g > 14) {
    if (b * 100 < g * 62) {
      return C_ROSA;     
    }
    else if (b * 100 < g * 85) {
      return C_ROJO;      
    }
    else {
      return C_NARANJA;   
    }
  }
  else {
    return C_NINGUNO;
  }
}

void mostrarColor() {
  int color = detectarColor();

  if (color == ultimoColor) {
    return;
  }

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);

  if (color == C_CIAN) {
    lcd.print("Turquesa / Azul");
    Serial.print ("Turquesa / Azul");
  }
  else if (color == C_AMARILLO) {
    lcd.print("Amarillo");
    Serial.print ("Amarillo");
  }
  else if (color == C_NARANJA) {
    lcd.print("Naranja");
    Serial.print ("Naranja");

  }
  else if (color == C_ROSA) {
    lcd.print("Rosa");
    Serial.print ("Rosa");

  }
  else if (color == C_ROJO) {
    lcd.print("Rojo");
    Serial.print ("Rojo");

  }
  else if (color == C_VERDE) {
    lcd.print("Verde");
    Serial.print ("Verde");

  }
  else {
    lcd.print("Sin identificar");
    Serial.print ("Sin identificar");

  }

  ultimoColor = color;
}

// Pista A
void pistaA() {
  delay(500);
  avanzarUnidad();

  medirTodos();
  mostrarColor();

  bool hayParedFrente = (distFrente < UMBRAL);
  bool hayParedIzquierda = (distIzquierda < UMBRAL);
  bool hayParedDerecha = (distDerecha < UMBRAL);

  if (hayParedFrente && hayParedIzquierda && hayParedDerecha) {
    delay(50);
    medirTodos();
    hayParedFrente = (distFrente < UMBRAL);
    hayParedIzquierda = (distIzquierda < UMBRAL);
    hayParedDerecha = (distDerecha < UMBRAL);
  }

  if (hayParedFrente && hayParedIzquierda && hayParedDerecha) {
    detener();
    delay(100);
    retroceder(VELOCIDAD);
    delay(1350);
    detener();
    if (distDerecha > distIzquierda) {
      girar90Derecha();
    }
    else if (distIzquierda < distDerecha) {
      girar90Izquierda();
    }

    return;
  }

  if (!hayParedFrente) {
  }
  else if (!hayParedDerecha) {
    girar90Derecha();
  }
  else if (!hayParedIzquierda) {
    girar90Izquierda();
  }
}

// Pista B
void pistaB() {
  if (seccionB == 1) {
    seccion1();
  }
  else if (seccionB == 2) {
    seccion2();
  }
  else if (seccionB == 3) {
    seccion3();
  }
  else {
    detener();
  }
}

void reiniciarPistaB() {
  seccionB = 1;
  iniciadoB = false;
  tieneBola = false;
  posicionAnillo = 0;
  rumbo = ARRIBA;
  abrirGarra();
  mostrarSeccion();
}

void mostrarSeccion() {
  lcd.setCursor(0, 0);
  lcd.print("Pista B Secc: ");
  lcd.print(seccionB);
  lcd.print(" ");
}

void buscarCheckpoint(int tiempoMaximo) {
  unsigned long inicio = millis();

  while (millis() - inicio < tiempoMaximo) {
    distFrente = medirUno(TRIG_F, ECHO_F);

    if (distFrente < DIST_MIN) {
      break;
    }

    avance();

    if (detectarColor() == C_ROJO) {
      break;
    }
  }

  detener();
  delay(100);
  avanzarTiempo(TIEMPO_UNIDAD / 2);
  delay(100);
}

// Seccion 1
void seccion1() {
  if (!iniciadoB) {
    mostrarSeccion();
    abrirGarra();
    avanzarUnidad();
    girar90Derecha();
    posicionAnillo = 0;
    iniciadoB = true;
    return;
  }

  if (!tieneBola) {
    pasoAnillo();

    if (posicionAnillo == 1 || posicionAnillo == 3 || posicionAnillo == 5 || posicionAnillo == 7) {
      revisarCentro();
    }
  }
  else if (posicionAnillo != 3) {
    pasoAnillo();
  }
  else {
    girar90Derecha();
    buscarCheckpoint(TIEMPO_UNIDAD * 2);
    seccionB = 2;
    iniciadoB = false;
  }
}

void pasoAnillo() {
  avanzarUnidad();

  posicionAnillo = posicionAnillo + 1;
  if (posicionAnillo == 8) {
    posicionAnillo = 0;
  }

  if (posicionAnillo == 0 || posicionAnillo == 2 || posicionAnillo == 4 || posicionAnillo == 6) {
    girar90Izquierda();
  }
}

void revisarCentro() {
  girar90Izquierda();
  medirTodos();

  // La entrada al centro esta abierta (el sensor si ve la pared del fondo)
  if (distFrente > UMBRAL_ENTRADA) {
    abrirGarra();

    // Avanzar por tiempo, sin depender de ver la pelota
    unsigned long inicio = millis();
    while (millis() - inicio < TIEMPO_PELOTA) {
      distFrente = medirUno(TRIG_F, ECHO_F);

      // Proteccion: no chocar con una pared
      if (distFrente < DIST_MIN) {
        break;
      }
      avance();
    }
    detener();
    delay(100);

    cerrarGarra();

    // Retroceder lo que se avanzo
    unsigned long duracion = millis() - inicio;
    retroceder(VELOCIDAD);
    delay(duracion);
    detener();
    delay(100);

    tieneBola = true;
  }

  girar90Derecha();
}

// Seccion 2
void seccion2() {
  if (!iniciadoB) {
    mostrarSeccion();
    avanzarTiempo(TIEMPO_UNIDAD);
    iniciadoB = true;
    return;
  }

  if (detectarColor() == C_ROJO) {
    detener();
    delay(100);
    avanzarTiempo(TIEMPO_UNIDAD / 2);
    seccionB = 3;
    iniciadoB = false;
    return;
  }

  bool lineaIzquierda = (digitalRead(IR_IZQ) == BLANCO);
  bool lineaDerecha = (digitalRead(IR_DER) == BLANCO);

  if (lineaIzquierda || lineaDerecha) {
    evadirLinea();
  }
  else {
    avance();
  }
}

void evadirLinea() {
  detener();
  delay(100);

  retroceder(VELOCIDAD);
  delay(250);
  detener();
  delay(100);

  medirTodos();

  if (distIzquierda > distDerecha) {
    girar90Izquierda();
    avanzarMediaUnidad();
    girar90Derecha();
  }
  else {
    girar90Derecha();
    avanzarMediaUnidad();
    girar90Izquierda();
  }
}

// Seccion 3
void seccion3() {
  if (!iniciadoB) {
    mostrarSeccion();
    rumbo = ARRIBA;
    avanzarUnidad();
    iniciadoB = true;
    return;
  }

  int color = detectarColor();

  if (color == C_VERDE) {
    detener();
    seccionB = 4;
    mostrarSeccion();
  }
  else if (color == C_CIAN) {
    girarARumbo(DERECHA);
    avanzarUnidad();
  }
  else if (color == C_AMARILLO) {
    girarARumbo(IZQUIERDA);
    avanzarUnidad();
  }
  else if (color == C_NARANJA) {
    girarARumbo(ARRIBA);
    avanzarUnidad();
  }
  else if (color == C_ROSA) {
    girarARumbo(ABAJO);
    avanzarUnidad();
  }
  else {
    detener();
    delay(50);
  }
}

void girarARumbo(int destino) {
  int cuartos = destino - rumbo;
  if (cuartos < 0) {
    cuartos = cuartos + 4;
  }

  if (cuartos == 1) {
    girar90Derecha();
  }
  else if (cuartos == 2) {
    girar180();
  }
  else if (cuartos == 3) {
    girar90Izquierda();
  }

  rumbo = destino;
}
