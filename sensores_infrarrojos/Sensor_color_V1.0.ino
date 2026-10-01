#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pines del Sensor de Color TCS230 / TCS3200
const int pinS0 = 2;
const int pinS1 = 3;
const int pinS2 = 4;
const int pinS3 = 11;
const int pinOut = 12;

int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

String lastDisplayedColor = "";

void setup() {
  // Inicialización de la pantalla LCD
  Wire.begin();      // Inicializa el bus I2C
  lcd.init();        // Inicializa la pantalla
  lcd.backlight();   // Enciende la luz de fondo
  lcd.setCursor(0, 0);
  lcd.print("Color Detectado:");

  // Configuración de pines de control del sensor
  pinMode(pinS0, OUTPUT);
  pinMode(pinS1, OUTPUT);
  pinMode(pinS2, OUTPUT);
  pinMode(pinS3, OUTPUT);
  pinMode(pinOut, INPUT);

  // Escala de frecuencia de salida al 20% (S0 HIGH, S1 LOW)
  digitalWrite(pinS0, HIGH);
  digitalWrite(pinS1, LOW);

  // Inicialización de la Terminal
  Serial.begin(9600);
}

void loop() {

  digitalWrite(pinS2, LOW);
  digitalWrite(pinS3, LOW);
  redFrequency = pulseIn(pinOut, LOW, 20000);
  delay(10);

  // Canal VERDE (S2 HIGH, S3 HIGH)
  digitalWrite(pinS2, HIGH);
  digitalWrite(pinS3, HIGH);
  greenFrequency = pulseIn(pinOut, LOW, 20000);
  delay(10);

  // Canal AZUL (S2 LOW, S3 HIGH)
  digitalWrite(pinS2, LOW);
  digitalWrite(pinS3, HIGH);
  blueFrequency = pulseIn(pinOut, LOW, 20000);
  delay(10);

  // 2. IDENTIFICACIÓN DE COLOR MEDIANTE CONDICIONALES DIRECTOS
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

  Serial.print("Rojo: ");
  Serial.print(redFrequency);
  Serial.print(" | Verde: ");
  Serial.print(greenFrequency);
  Serial.print(" | Azul: ");
  Serial.print(blueFrequency);
  Serial.print("  --> Color: ");
  Serial.println(detectedColor);

  if (detectedColor != lastDisplayedColor) {
    
    lcd.setCursor(0, 1);
    lcd.print(detectedColor);
    lastDisplayedColor = detectedColor;
  }

  delay(1000);
}