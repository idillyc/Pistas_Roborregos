 #include <LiquidCrystal_I2C.h>

// sensores infrarrojos
  const int IR_IZQ = 30;
  const int IR_DER = 31;

// LEDS de indicacion
  const int pinS0 = 2;
  const int pinS1 = 3;
  const int pinS2 = 4;
  const int pinS3 = 11;
  const int pinOut = 12;
  int redFrequency = 0;
  int greenFrequency = 0;
  int blueFrequency = 0;







void setup() {
 
}

void loop() {
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


  Serial.print("R:"); Serial.print(redFrequency);
  Serial.print(" G:"); Serial.print(greenFrequency);
  Serial.print(" B:"); Serial.println(blueFrequency);


  delay(1000);
   }
}
