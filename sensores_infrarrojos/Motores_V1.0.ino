// Pines de control de motores (PWM directo en los pines IN)
const int IN1 = 9;  
const int IN2 = 8;
const int IN3 = 6;  
const int IN4 = 7;

// Velocidades de prueba (0 a 255)
const int VELOCIDAD = 180;
const int VELOCIDAD_GIRO = 150;

// Declaración de funciones
void avanzar(int velocidad);
void retroceder(int velocidad);
void girarDerecha(int velocidad);
void girarIzquierda(int velocidad);
void detener();

void setup() {
  // Configuración de pines de motores como salidas
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Garantizar que inicien apagados
  detener();
  delay(1000); // Pausa inicial de 1 segundo
}

void loop() {
  // 1. Avanzar por 2 segundos
  avanzar(VELOCIDAD);
  delay(2000);

  // 2. Detener brevemente
  detener();
  delay(500);

  // 3. Girar a la derecha por 1 segundo
  girarDerecha(VELOCIDAD_GIRO);
  delay(1000);

  // 4. Detener brevemente
  detener();
  delay(500);

  // 5. Avanzar por 2 segundos
  avanzar(VELOCIDAD);
  delay(2000);

  // 6. Detener brevemente
  detener();
  delay(500);

  // 7. Girar a la izquierda por 1 segundo
  girarIzquierda(VELOCIDAD_GIRO);
  delay(1000);

  // 8. Detención total de 3 segundos antes de repetir el ciclo
  detener();
  delay(3000);
}

// ================= FUNCIONES DE MOVIMIENTO =================

void avanzar(int velocidad) {
  analogWrite(IN1, velocidad);
  digitalWrite(IN2, LOW);
  analogWrite(IN3, velocidad);
  digitalWrite(IN4, LOW);
}

void retroceder(int velocidad) {
  digitalWrite(IN1, LOW);
  analogWrite(IN2, velocidad);
  digitalWrite(IN3, LOW);
  analogWrite(IN4, velocidad);
}

void girarDerecha(int velocidad) {
  analogWrite(IN1, velocidad);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  analogWrite(IN4, velocidad);
}

void girarIzquierda(int velocidad) {
  digitalWrite(IN1, LOW);
  analogWrite(IN2, velocidad);
  analogWrite(IN3, velocidad);
  digitalWrite(IN4, LOW);
}

void detener() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}


hola mundo 