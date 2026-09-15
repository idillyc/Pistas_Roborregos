void setup() {
  {

  const int IN1 = 8;
  const int IN2 = 9;
  const int ENA = 10;
  const int IN3 = 6;
  const int IN4 = 7;
  const int ENB = 5;
  
  const int TRIG_F = 22;
  const int ECHO_F = 23;
  const int TRIG_I = 24;
  const int ECHO_I = 25;
  const int TRIG_D = 26;
  const int ECHO_D = 27;

  const int UMBRAL = 5;      
  const int VELOCIDAD = 180;  // 0-255
  const int VELOCIDAD_GIRO = 150;

  long distFrente, distIzquierda, distDerecha;

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(TRIG_F, OUTPUT);
  pinMode(ECHO_F, INPUT);
  pinMode(TRIG_I, OUTPUT);
  pinMode(ECHO_I, INPUT);
  pinMode(TRIG_D, OUTPUT);
  pinMode(ECHO_D, INPUT);

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
  delay(10); 

  distIzquierda = medirUno(TRIG_I, ECHO_I);
  delay(10);

  distDerecha = medirUno(TRIG_D, ECHO_D);
  delay(10);
}


