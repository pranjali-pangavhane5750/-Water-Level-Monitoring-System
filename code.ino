#include <Wire.h>              // Include the Wire library for I2C
#include <LiquidCrystal_I2C.h>  // Include the LiquidCrystal_I2C library for LCD

#define MOTOR_PIN 9     // GPIO pin to control the motor
#define TRIG_PIN 7      // GPIO pin for Ultrasonic sensor TRIG
#define ECHO_PIN 6      // GPIO pin for Ultrasonic sensor ECHO

// Define distances (in centimeters) for an empty and full tank
const float MAX_DISTANCE = 18.0;  // Max distance when tank is empty
const float MIN_DISTANCE = 3.0;   // Min distance when tank is full

const int TURN_ON_PERCENTAGE = 20;   // Turn on motor when water is below 20%
const int TURN_OFF_PERCENTAGE = 80;  // Turn off motor when water is above 80%

long duration;
float distance, waterLevelPercentage;

// Initialize the LCD (address 0x27, 16 columns and 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(MOTOR_PIN, OUTPUT);       // Set motor pin as output
  pinMode(TRIG_PIN, OUTPUT);        // Set TRIG pin as output
  pinMode(ECHO_PIN, INPUT);         // Set ECHO pin as input
  digitalWrite(MOTOR_PIN, LOW);     // Initially turn off the motor
  
  Serial.begin(115200);             // Start serial communication for debugging

  // Initialize the LCD
  lcd.init();
  lcd.backlight();  // Turn on the backlight of the LCD
  lcd.clear();      // Clear the screen
}

void loop() {
  // Send an ultrasonic pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure the time it takes for the echo to return
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration * 0.0343) / 2;  // Convert duration to distance in cm

  // Calculate water level percentage
  if (distance > MAX_DISTANCE) {
    distance = MAX_DISTANCE;  // Cap at max distance
  }
  if (distance < MIN_DISTANCE) {
    distance = MIN_DISTANCE;  // Cap at min distance
  }

  waterLevelPercentage = (1 - (distance - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE)) * 100;

  // Print the distance and percentage for debugging
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm, Water Level: ");
  Serial.print(waterLevelPercentage);
  Serial.println("%");

  // Display on LCD
  lcd.clear();  // Clear the previous display
  lcd.setCursor(0, 0);  // Set the cursor to the first row, first column
  lcd.print("Water Level:");
  lcd.setCursor(0, 1);  // Set the cursor to the second row
  lcd.print(waterLevelPercentage);
  lcd.print("%");

  // Control the motor based on the water level percentage
  if (waterLevelPercentage < TURN_ON_PERCENTAGE) {
    digitalWrite(MOTOR_PIN, HIGH);  // Turn on motor
    lcd.setCursor(0, 1);  // Display status on LCD
    lcd.print("Motor: ON ");
    Serial.println("Water level low: Motor ON");
  } 
  else if (waterLevelPercentage > TURN_OFF_PERCENTAGE) {
    digitalWrite(MOTOR_PIN, LOW);   // Turn off motor
    lcd.setCursor(0, 1);  // Display status on LCD
    lcd.print("Motor: OFF");
    Serial.println("Water level high: Motor OFF");
  }

  delay(1000);  // Wait 1 second before the next reading
}
