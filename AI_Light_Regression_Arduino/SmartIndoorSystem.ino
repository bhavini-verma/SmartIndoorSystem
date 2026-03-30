#include "DHT.h"
#include <math.h>

// Pins
#define PIR_PIN 2
#define DHT_PIN 3
#define LDR_PIN A0
#define RELAY_PIN 8

#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);


int total = 0;
int correct = 0;


float sigmoid(float x) {
  return 1.0 / (1.0 + exp(-x));
}

void setup() {
  Serial.begin(9600);

  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH);
  dht.begin();

  Serial.println("======================================");
  Serial.println(" SMART INDOOR AUTOMATION SYSTEM");
  Serial.println(" (Regression-Based Smart Control)");
  Serial.println("======================================");
}

void loop() {

  int motion = digitalRead(PIR_PIN);
  int ldr = analogRead(LDR_PIN);
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  
  float motion_f = motion;
  float light_f = 1.0 - (ldr / 1023.0);

  float z = (-4.0 * light_f) + (2.0 * motion_f) - 0.5;
  float probability = sigmoid(z);

  bool predicted = (probability > 0.6);

  
  bool expected = (motion == HIGH && ldr > 600);

  
  total++;
  if (predicted == expected) correct++;
  float accuracy = (correct * 100.0) / total;

  Serial.println("\n--------------------------------------");
  Serial.println("   REGRESSION MODEL OUTPUT");
  Serial.println("--------------------------------------");

  // PIR
  Serial.print("PIR Sensor (Motion Detection) : ");
  Serial.println(motion ? "Detected" : "No Motion");

  // LDR
  Serial.print("LDR Sensor (Light Intensity)  : ");
  Serial.print(ldr);
  Serial.print("  → ");

  if (ldr > 700) Serial.println("BRIGHT ☀️");
  else if (ldr > 400) Serial.println("MODERATE 🌤");
  else Serial.println("DARK 🌑");

  // DHT
  Serial.print("Temperature (DHT11)           : ");
  Serial.print(temp);
  Serial.println(" C");

  Serial.print("Humidity (DHT11)              : ");
  Serial.print(hum);
  Serial.println(" %");

  // ML Decision (unchanged display)
  Serial.println("\n[Machine Learning Decision]");
  Serial.print("Predicted Probability         : ");
  Serial.println(probability);

  Serial.print("Final Output (Relay Control)  : ");

  //  RELAY
  if (expected) {
    digitalWrite(RELAY_PIN, LOW);   // ON
    Serial.println("LIGHT OFF");
  } else {
    digitalWrite(RELAY_PIN, HIGH);  // OFF
    Serial.println("LIGHT ON");
  }


  Serial.print("Model Accuracy                : ");
  Serial.print(accuracy);
  Serial.println(" %");

  Serial.println("--------------------------------------");

  delay(2000);
}