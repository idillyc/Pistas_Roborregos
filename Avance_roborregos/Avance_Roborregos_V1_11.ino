#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo miServo;

// ================= PINES =================
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

// ================= AJUSTES =================
const int VELOCIDAD = 95;
const int VELOCIDAD_GIRO = 100;
const int AJUSTE_IZQ = 20;

const int TIEMPO_UNIDAD = 1350;
const int TIEMPO_GIRO = 650;
const int TIEMPO_GIRO_180 = 1300;

const int UMBRAL = 12;
const int UMBRAL_ENTRADA = 20;
const int DIST_MIN = 5;
const int TIEMPO_PELOTA = 700;
const int MAX_VUELTAS = 2;

const int GARRA_ABIERTA = 0;
const int GARRA_CERRADA = 90;

const int BLANCO = LOW;
const bool DEBUG = false;
const bool PARAR_EN_META = true;
const unsigned long DEBOUNCE = 50;

// ================= COLORES Y DIRECCIONES =================
const int C_NINGUNO = 0;
const int C_CIAN = 1;
const int C_AMARILLO = 2;
const int C_NARANJA = 3;
const int C_ROSA = 4;
const int C_ROJO = 5;
const int C_VERDE = 6;

const int ARRIBA = 0;
const int DERECHA = 1;
const int ABAJO = 2;
const int IZQUIERDA = 3;

// ================= VARIABLES =================
int modo = 0;
long distFrente;
long distIzquierda;
long distDerecha;
int ultimoColor = -1;

int seccionB = 1;
bool iniciadoB = false;
bool tieneBola = false;
int posicionAnillo = 0;
int vueltas = 0;
int sinColor = 0;
int rumbo = ARRIBA;

// ================= MOTORES =================
void mover(int izq, int der){
  analogWrite(IN1, max(izq, 0));
  analogWrite(IN2, max(-izq, 0));
  analogWrite(IN3, max(der, 0));
  analogWrite(IN4, max(-der, 0));
}

void avance(){
  mover(VELOCIDAD - AJUSTE_IZQ, VELOCIDAD);
}

void retroceder(int vel){
  mover(-(vel - AJUSTE_IZQ), -vel);
}

void giroIzquierda(){
  mover(-(VELOCIDAD_GIRO - AJUSTE_IZQ), VELOCIDAD_GIRO);
}

void giroDerecha(){
  mover(VELOCIDAD_GIRO - AJUSTE_IZQ, -VELOCIDAD_GIRO);
}

void detener(){
  mover(0, 0);
}

void girar(bool derecha, int tiempo){
  detener();
  delay(100);
  if (derecha){
    giroDerecha();
  }else{
    giroIzquierda();
  }
  delay(tiempo);
  detener();
  delay(100);
}

void girar90Derecha(){
  girar(true, TIEMPO_GIRO);
  rumbo = (rumbo + 1) % 4;
}

void girar90Izquierda(){
  girar(false, TIEMPO_GIRO);
  rumbo = (rumbo + 3) % 4;
}

void girar180(){
  girar(true, TIEMPO_GIRO_180);
  rumbo = (rumbo + 2) % 4;
}

// ================= GARRA =================
void cerrarGarra(){
  miServo.write(GARRA_CERRADA);
  delay(500);
}

void abrirGarra(){
  miServo.write(GARRA_ABIERTA);
  delay(500);
}

// ================= ULTRASÓNICOS =================
long medirUno(int pinTrig, int pinEcho){
  digitalWrite(pinTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);

  long duracion = pulseIn(pinEcho, HIGH, 20000);
  if (duracion == 0){
    return 999;
  }
  return duracion * 0.034 / 2;
}

long medirFiltrado(int pinTrig, int pinEcho){
  long a = medirUno(pinTrig, pinEcho);
  delay(10);
  long b = medirUno(pinTrig, pinEcho);
  delay(10);
  long c = medirUno(pinTrig, pinEcho);
  return max(min(a, b), min(max(a, b), c));
}

void medirTodos(){
  distFrente = medirFiltrado(TRIG_F, ECHO_F);
  distIzquierda = medirFiltrado(TRIG_I, ECHO_I);
  distDerecha = medirFiltrado(TRIG_D, ECHO_D);
}

void avanzarTiempo(int tiempo){
  avance();
  delay(tiempo);
  detener();
}

void avanzarSeguro(int tiempo){
  unsigned long inicio = millis();
  while (millis() - inicio < tiempo){
    if (medirUno(TRIG_F, ECHO_F) < DIST_MIN){
      break;
    }
    avance();
    delay(10);
  }
  detener();
  delay(100);
}

void avanzarUnidad(){
  avanzarSeguro(TIEMPO_UNIDAD);
}

void avanzarMediaUnidad(){
  avanzarSeguro(TIEMPO_UNIDAD / 2);
}

// ================= COLOR =================
int detectarColor(){
  digitalWrite(pinS2, LOW);
  digitalWrite(pinS3, LOW);
  long r = pulseIn(pinOut, LOW, 20000);

  digitalWrite(pinS2, HIGH);
  digitalWrite(pinS3, HIGH);
  long g = pulseIn(pinOut, LOW, 20000);

  digitalWrite(pinS2, LOW);
  digitalWrite(pinS3, HIGH);
  long b = pulseIn(pinOut, LOW, 20000);

  if (DEBUG){
    Serial.print("R: ");
    Serial.print(r);
    Serial.print("  G: ");
    Serial.print(g);
    Serial.print("  B: ");
    Serial.println(b);
  }

  if (r == 0 || g == 0 || b == 0){
    return C_NINGUNO;
  }

  if (r >= 20 && g * 100 < r * 85 && b * 100 >= g * 105){
    return C_VERDE;
  }

  if (r >= 17 && r < 32 && g * 10 < r * 7 && b <= g){
    return C_CIAN;
  }

  if (r <= 14 && g <= 14 && g * 10 <= r * 13){
    return C_AMARILLO;
  }

  if (r <= 14 && g * 10 > r * 13){
    if (b * 100 < g * 62){
      return C_ROSA;
    }
    if (b * 100 < g * 85){
      return C_ROJO;
    }
    return C_NARANJA;
  }

  return C_NINGUNO;
}

int leerColor(){
  int a = detectarColor();
  int b = detectarColor();
  if (a == b){
    return a;
  }
  int c = detectarColor();
  if (c == a || c == b){
    return c;
  }
  return C_NINGUNO;
}

bool veRojo(int veces){
  for (int i = 0; i < veces; i++){
    if (detectarColor() != C_ROJO){
      return false;
    }
  }
  return true;
}

const char* nombreColor(int color){
  switch (color){
    case C_CIAN:
      return "Turquesa / Azul";
    case C_AMARILLO:
      return "Amarillo";
    case C_NARANJA:
      return "Naranja";
    case C_ROSA:
      return "Rosa";
    case C_ROJO:
      return "Rojo";
    case C_VERDE:
      return "Verde";
    default:
      return "Sin identificar";
  }
}

void mostrarColor(int color){
  if (color == ultimoColor){
    return;
  }
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(nombreColor(color));
  if (DEBUG){
    Serial.println(nombreColor(color));
  }
  ultimoColor = color;
}


void pistaA(){
  medirTodos();
  bool paredFrente = (distFrente < UMBRAL);
  bool paredIzquierda = (distIzquierda < UMBRAL);
  bool paredDerecha = (distDerecha < UMBRAL);

  if (paredFrente && paredIzquierda && paredDerecha) {
    delay(50);
    medirTodos();
    paredFrente = (distFrente < UMBRAL);
    paredIzquierda = (distIzquierda < UMBRAL);
    paredDerecha = (distDerecha < UMBRAL);
  }

  if (paredFrente && paredIzquierda && paredDerecha) {
    detener();
    delay(100);
    retroceder(VELOCIDAD);
    delay(1450);
    detener();
  }

  if (!paredDerecha){
    girar90Derecha();
  }else if (!paredFrente){
    // sigue derecho
  }else if (!paredIzquierda){
    girar90Izquierda();
  }else{
    girar180();
  }

  avanzarUnidad();

  int color = leerColor();
  mostrarColor(color);

  if (PARAR_EN_META && color == C_ROJO){
    detener();
    lcd.setCursor(0, 0);
    lcd.print("Pista A: META :)  ");
    modo = 0;
  }
}

// ================= PISTA B =================
void mostrarSeccion(){
  lcd.setCursor(0, 0);
  lcd.print("Pista B Secc: ");
  lcd.print(seccionB);
  lcd.print(" ");
}

// Sección 1: busca la pelota dando vueltas al anillo de 8 casillas
void pasoAnillo(){
  avanzarUnidad();
  posicionAnillo = (posicionAnillo + 1) % 8;
  if (posicionAnillo == 0){
    vueltas++;
  }
  if (posicionAnillo % 2 == 0){
    girar90Izquierda();
  }
}

void revisarCentro(){
  girar90Izquierda();
  medirTodos();

  if (distFrente > UMBRAL_ENTRADA){
    unsigned long inicio = millis();
    while (millis() - inicio < TIEMPO_PELOTA){
      if (medirUno(TRIG_F, ECHO_F) < DIST_MIN){
        break;
      }
      avance();
      delay(10);
    }
    unsigned long duracion = millis() - inicio;
    detener();
    delay(100);

    cerrarGarra();

    retroceder(VELOCIDAD);
    delay(duracion);
    detener();
    delay(100);

    tieneBola = true;
  }
  girar90Derecha();
}

void buscarCheckpoint(int tiempoMaximo){
  unsigned long inicio = millis();
  bool encontrado = false;

  while (millis() - inicio < tiempoMaximo){
    if (medirUno(TRIG_F, ECHO_F) < DIST_MIN){
      break;
    }
    avance();
    if (veRojo(3)){
      encontrado = true;
      break;
    }
  }
  detener();
  delay(100);

  if (encontrado){
    avanzarTiempo(TIEMPO_UNIDAD / 2);
  }
  delay(100);
}

void seccion1(){
  if (!iniciadoB){
    avanzarUnidad();
    girar90Derecha();
    posicionAnillo = 0;
    vueltas = 0;
    iniciadoB = true;
    return;
  }

  bool buscando = !tieneBola && vueltas < MAX_VUELTAS;

  if (buscando){
    pasoAnillo();
    if (posicionAnillo % 2 == 1){
      revisarCentro();
    }
  }else if (posicionAnillo != 3){
    pasoAnillo();
  }else{
    girar90Derecha();
    buscarCheckpoint(TIEMPO_UNIDAD * 2);
    seccionB = 2;
    iniciadoB = false;
  }
}

// Sección 2
void evadirLinea(bool lineaIzq, bool lineaDer){
  detener();
  delay(100);
  retroceder(VELOCIDAD);
  delay(250);
  detener();
  delay(100);

  bool haciaIzquierda;
  if (lineaDer && !lineaIzq){
    haciaIzquierda = true;
  }else if (lineaIzq && !lineaDer){
    haciaIzquierda = false;
  }else{
    medirTodos();
    haciaIzquierda = (distIzquierda > distDerecha);
  }

  if (haciaIzquierda){
    girar90Izquierda();
    avanzarMediaUnidad();
    girar90Derecha();
  }else{
    girar90Derecha();
    avanzarMediaUnidad();
    girar90Izquierda();
  }
}

void seccion2(){
  if (!iniciadoB){
    mostrarSeccion();
    avanzarTiempo(TIEMPO_UNIDAD);
    iniciadoB = true;
    return;
  }

  if (veRojo(3)){
    detener();
    delay(100);
    avanzarTiempo(TIEMPO_UNIDAD / 2);
    seccionB = 3;
    iniciadoB = false;
    return;
  }

  bool lineaIzq = (digitalRead(IR_IZQ) == BLANCO);
  bool lineaDer = (digitalRead(IR_DER) == BLANCO);

  if (lineaIzq || lineaDer){
    evadirLinea(lineaIzq, lineaDer);
  }else{
    avance();
  }
}

// Sección 3
void girarARumbo(int destino){
  int cuartos = (destino - rumbo + 4) % 4;
  if (cuartos == 1){
    girar90Derecha();
  }else if (cuartos == 2){
    girar180();
  }else if (cuartos == 3){
    girar90Izquierda();
  }
}

void seccion3(){
  if (!iniciadoB){
    mostrarSeccion();
    rumbo = ARRIBA;
    sinColor = 0;
    avanzarUnidad();
    iniciadoB = true;
    return;
  }

  int color = leerColor();

  if (color == C_VERDE){
    detener();
    seccionB = 4;
    lcd.setCursor(0, 0);
    lcd.print("Pista B: FIN!   ");
    return;
  }

  if (color == C_NINGUNO || color == C_ROJO){
    if (sinColor < 3){
      avanzarTiempo(TIEMPO_UNIDAD / 8);
      sinColor++;
    }else{
      detener();
      delay(50);
    }
    return;
  }

  sinColor = 0;
  if (color == C_CIAN){
    girarARumbo(DERECHA);
  }else if (color == C_AMARILLO){
    girarARumbo(IZQUIERDA);
  }else if (color == C_NARANJA){
    girarARumbo(ARRIBA);
  }else if (color == C_ROSA){
    girarARumbo(ABAJO);
  }
  avanzarUnidad();
}

void pistaB(){
  if (seccionB == 1){
    seccion1();
  }else if (seccionB == 2){
    seccion2();
  }else if (seccionB == 3){
    seccion3();
  }else{
    detener();
    modo = 0;
  }
}

// ================= BOTONES =================
bool presionado(int pin){
  if (digitalRead(pin) == HIGH){
    return false;
  }
  delay(DEBOUNCE);
  return digitalRead(pin) == LOW;
}

void esperarSoltar(int pin){
  while (digitalRead(pin) == LOW){
  }
  delay(DEBOUNCE);
}

void iniciarPistaA(){
  lcd.clear();
  lcd.print("Pista A");
  ultimoColor = -1;
  delay(1000);
  modo = 1;
}

void iniciarPistaB(int seccion){
  seccionB = seccion;
  iniciadoB = false;
  tieneBola = false;
  posicionAnillo = 0;
  vueltas = 0;
  rumbo = ARRIBA;
  if (seccion == 1){
    abrirGarra();
  }else{
    cerrarGarra();
  }
  mostrarSeccion();
  lcd.setCursor(0, 1);
  lcd.print("                ");
  modo = 2;
}


void elegirSeccionB(){
  int seccion = 1;
  unsigned long ultimoToque = millis();

  while (true){
    seccionB = seccion;
    lcd.clear();
    mostrarSeccion();
    lcd.setCursor(0, 1);
    lcd.print("Inicia en 2 seg");

    while (millis() - ultimoToque < 2000){
      if (presionado(BTNb)){
        esperarSoltar(BTNb);
        seccion = seccion % 3 + 1;
        ultimoToque = millis();
        break;
      }
    }
    if (millis() - ultimoToque >= 2000){
      break;
    }
  }
  iniciarPistaB(seccion);
}

void leerBotones(){
  if (presionado(BTNa)){
    esperarSoltar(BTNa);
    iniciarPistaA();
  }else if (presionado(BTNb)){
    esperarSoltar(BTNb);
    elegirSeccionB();
  }
}

// ================= PROGRAMA PRINCIPAL =================
void setup(){
  lcd.init();
#ifdef WIRE_HAS_TIMEOUT
  Wire.setWireTimeout(3000, true);
#endif
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Selecciona Pista");
  lcd.setCursor(0, 1);
  lcd.print("BTNa=A    BTNb=B");

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  detener();

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

void loop(){
  if (modo == 1){
    pistaA();
  }else if (modo == 2){
    pistaB();
  }else{
    leerBotones();
  }
}
