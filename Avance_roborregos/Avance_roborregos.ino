

 #include <Wire.h>
 #include <LiquidCrystal_I2C.h>

 LiquidCrystal_I2C lcd(0x27, 16, 2);

 // pines de los motores
  const int IN1 = 8;
  const int IN2 = 9;
  const int ENA = 10;
  const int IN3 = 6;
  const int IN4 = 7;
  const int ENB = 5;

// pines sensores ultrasonicos 
  const int TRIG_F = 22;
  const int ECHO_F = 23;
  const int TRIG_I = 24;
  const int ECHO_I = 25;
  const int TRIG_D = 26;
  const int ECHO_D = 27;

// pines sensores infrarrojos
  const int IR_IZQ = 30;
  const int IR_DER = 31;

// LEDS de indicacion
  const int pinS0 = 2;
  const int pinS1 = 3;
  const int pinS2 = 4;
  const int pinS3 = 11;
  const int pinOut = 12;

//velocidad de los motores
  const int UMBRAL = 5;      
  const int VELOCIDAD = 180;  // 0-255
  const int VELOCIDAD_GIRO = 150;

  long distFrente, distIzquierda, distDerecha;

// variables canales rgb
  int redFrequency = 0;
  int greenFrequency = 0;
  int blueFrequency = 0;

  String lastDisplayedColor = "";

// determinante de funciones
void avanzar(int velocidad);
void retroceder(int velocidad);
void girarDerecha(int velocidad);
void girarIzquierda(int velocidad);
void detener();
long medirUno(int pinTrig, int pinEcho);
void medirTodos();

void setup() {

 // Inicialización de la pantalla LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Color Detectado:");
 
 // Configuración de pines de motores como salidas
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
 
// Configuración de pines de ultrasonidos
  pinMode(TRIG_F, OUTPUT);
  pinMode(ECHO_F, INPUT);
  pinMode(TRIG_I, OUTPUT);
  pinMode(ECHO_I, INPUT);
  pinMode(TRIG_D, OUTPUT);
  pinMode(ECHO_D, INPUT);

 // Configuración de pines del Sensor de Color
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
  medirTodos();

  Serial.print("F:"); Serial.print(distFrente);
  Serial.print(" I:"); Serial.print(distIzquierda);
  Serial.print(" D:"); Serial.println(distDerecha);

  bool hayParedFrente = (distFrente < UMBRAL);
  bool hayParedIzquierda = (distIzquierda < UMBRAL);
  bool hayParedDerecha = (distDerecha < UMBRAL);

  if (!hayParedFrente) {
    avanzar(VELOCIDAD);
  }
  else if (!hayParedDerecha) {
    detener();
    delay(100);
    girarDerecha(VELOCIDAD_GIRO);
    delay(400);
    detener();
  }
  else if (!hayParedIzquierda) {
    detener();
    delay(100);
    girarIzquierda(VELOCIDAD_GIRO);
    delay(400);
    detener();
  }
  else {
    detener();
    delay(100);
    girarDerecha(VELOCIDAD_GIRO);
    delay(800);
    detener();
  }
  {
  digitalWrite(pinS2, LOW);
  digitalWrite(pinS3, LOW);
  redFrequency = pulseIn(pinOut, LOW);
  delay(20);

  digitalWrite(pinS2, HIGH);
  digitalWrite(pinS3, HIGH);
  greenFrequency = pulseIn(pinOut, LOW);
  delay(20);

  digitalWrite(pinS2, LOW);
  digitalWrite(pinS3, HIGH);
  blueFrequency = pulseIn(pinOut, LOW);
  delay(20);

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


  Serial.print("R:"); Serial.print(redFrequency);
  Serial.print(" G:"); Serial.print(greenFrequency);
  Serial.print(" B:"); Serial.println(blueFrequency);
}
//Movimiento

void avanzar(int velocidad) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, velocidad);
  analogWrite(ENB, velocidad);
}

void retroceder(int velocidad) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, velocidad);
  analogWrite(ENB, velocidad);
}

void girarDerecha(int velocidad) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, velocidad);
  analogWrite(ENB, velocidad);
}

void girarIzquierda(int velocidad) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, velocidad);
  analogWrite(ENB, velocidad);
}

void detener() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

//sensores

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
  distIzquierda = medirUno(TRIG_I, ECHO_I); 
  distDerecha = medirUno(TRIG_D, ECHO_D);
}




