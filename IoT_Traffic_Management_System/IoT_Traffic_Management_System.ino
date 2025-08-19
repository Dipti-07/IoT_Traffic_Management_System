/****************************************************
 * IoT Traffic Management System
 * Hardware: NodeMCU ESP8266, HC-SR04 Ultrasonic Sensor,
 *           6 LEDs (Red, Yellow, Green for two roads)
 * Control: Blynk dashboard (V1–V8)
 * Description: Controls traffic lights based on distance 
 * measurement.  
 ****************************************************/

#define BLYNK_TEMPLATE_ID "TMPL6LDztxWwX"
#define BLYNK_TEMPLATE_NAME "Traffic Management System"
#define BLYNK_AUTH_TOKEN "hcybGa_5nb1behpkPltlaVfYoA8j5-2Q"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// ===== Wi-Fi Credentials =====
char ssid[] = "Class_B3";
char pass[] = "987654321";

// ===== Hardware Pin Mapping =====
// Road 1 LEDs
const int yellow1 = D0;
const int red1    = D1;
const int green1  = D2;

// Road 2 LEDs
const int yellow2 = D5;
const int green2  = D4;
const int red2    = D3;

// Ultrasonic Sensor
const int trigPin = D6;
const int echoPin = D7;                                                                                             

// ===== Variables =====
long duration;            // Duration of pulse from sensor
int distance;             // Measured distance in cm
int distanceThreshold = 30; // Threshold distance adjustable via Blynk
bool road1Active = true;    // Tracks which road is currently green

// ===== Blynk Virtual Pins =====
// V1 - Live distance display
// V2 - Distance threshold slider
// V3–V8 - LED widgets matching physical LEDs

// ===== Blynk Slider to set distance threshold =====
BLYNK_WRITE(V2) {
  distanceThreshold = param.asInt(); // Update threshold
  Serial.print("Distance threshold updated to: ");
  Serial.println(distanceThreshold);
}

// ===== Setup =====
void setup() {
  Serial.begin(115200);

  // Connect to Blynk server
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Configure LED pins as OUTPUT
  pinMode(green1, OUTPUT); pinMode(yellow1, OUTPUT); pinMode(red1, OUTPUT);
  pinMode(green2, OUTPUT); pinMode(yellow2, OUTPUT); pinMode(red2, OUTPUT);

  // Configure ultrasonic sensor pins
  pinMode(trigPin, OUTPUT); pinMode(echoPin, INPUT);

  // Initialize with Road 1 green, Road 2 red
  setRoad1Green();
}

// ===== Main loop =====
void loop() {
  Blynk.run();           // Run Blynk
  distance = getDistance(); // Read distance from HC-SR04

  Blynk.virtualWrite(V1, distance); // Update live distance in Blynk
  Serial.print("Distance detected: "); Serial.print(distance); Serial.println(" cm");

  // Traffic logic: switch roads based on distance threshold
  if (road1Active && distance >= distanceThreshold) switchToRoad2();
  else if (!road1Active && distance > 0 && distance < distanceThreshold) switchToRoad1();

  delay(1000); // Delay for sensor stability
}

// ===== Function: Measure distance using HC-SR04 =====
int getDistance() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000); // Timeout 30ms
  if (duration == 0) return -1;             // No object detected

  return duration * 0.034 / 2;             // Convert to cm
}

// ===== Functions to control traffic lights =====
void setRoad1Green() {
  // Physical LEDs
  digitalWrite(green1, HIGH); digitalWrite(yellow1, LOW); digitalWrite(red1, LOW);
  digitalWrite(green2, LOW); digitalWrite(yellow2, LOW); digitalWrite(red2, HIGH);

  // Update Blynk LEDs
  Blynk.virtualWrite(V3, 255); Blynk.virtualWrite(V4, 0); Blynk.virtualWrite(V5, 0);
  Blynk.virtualWrite(V6, 0);   Blynk.virtualWrite(V7, 0); Blynk.virtualWrite(V8, 255);
}

void setRoad2Green() {
  // Physical LEDs
  digitalWrite(green2, HIGH); digitalWrite(yellow2, LOW); digitalWrite(red2, LOW);
  digitalWrite(green1, LOW); digitalWrite(yellow1, LOW); digitalWrite(red1, HIGH);

  // Update Blynk LEDs
  Blynk.virtualWrite(V3, 0);   Blynk.virtualWrite(V4, 0); Blynk.virtualWrite(V5, 255);
  Blynk.virtualWrite(V6, 255); Blynk.virtualWrite(V7, 0); Blynk.virtualWrite(V8, 0);
}

// ===== Functions to handle yellow light transition =====
void switchToRoad2() {
  digitalWrite(green1, LOW); digitalWrite(yellow1, HIGH); // Yellow before switch
  Blynk.virtualWrite(V3, 0); Blynk.virtualWrite(V4, 255);
  delay(2000); // Yellow duration
  digitalWrite(yellow1, LOW);
  setRoad2Green();
  road1Active = false;
  Serial.println("Switched to Road 2");
}

void switchToRoad1() {
  digitalWrite(green2, LOW); digitalWrite(yellow2, HIGH); // Yellow before switch
  Blynk.virtualWrite(V6, 0); Blynk.virtualWrite(V7, 255);
  delay(2000); // Yellow duration
  digitalWrite(yellow2, LOW);
  setRoad1Green();
  road1Active = true;
  Serial.println("Switched to Road 1");
}
 