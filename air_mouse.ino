#include <Wire.h>
#include <MPU6050_tockn.h>
MPU6050 mpu6050(Wire);

// Button setup
const int buttonPin = 2;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const int debounceDelay = 50;

// Sensitivity adjustment
const float sensitivity = 0.5; // Increase for faster cursor movement

// Scroll setup
const float SCROLL_THRESHOLD = 100.0; // Degrees/second sensitivity (adjust as needed)
const int SCROLL_INTERVAL = 100;      // Time between scroll events (ms)
unsigned long lastScrollTime = 0;

// Smooth scrolling filter
#define FILTER_SIZE 5
float gyroZBuffer[FILTER_SIZE];
byte filterIndex = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true); // Calibrate gyro (keep sensor flat)
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  mpu6050.update();
  
  // ========== MOUSE MOVEMENT ==========
  // Get filtered angle values
  float angleX = mpu6050.getAngleX() * sensitivity;
  float angleY = mpu6050.getAngleY() * sensitivity;

  // Map angles to mouse movement (invert Y axis)
  int deltaX = constrain(angleY, -30, 30); // Swap X/Y for natural movement
  int deltaY = constrain(angleX, -30, 30);
  
  // Dead zone to prevent drift
  if(abs(deltaX) < 2) deltaX = 0;
  if(abs(deltaY) < 2) deltaY = 0;

  // Send mouse movement
  Serial.print(deltaX);
  Serial.print(',');
  Serial.println(deltaY);

  // ========== BUTTON CLICK ==========
  // Handle button click with debouncing
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW) {
      Serial.println("CLICK");
    }
  }
  lastButtonState = reading;

  // ========== SCROLL FUNCTIONALITY ==========
  // Smooth the gyro data
  gyroZBuffer[filterIndex] = mpu6050.getGyroZ();
  filterIndex = (filterIndex + 1) % FILTER_SIZE;

  float filteredGyroZ = 0;
  for(int i=0; i<FILTER_SIZE; i++) {
      filteredGyroZ += gyroZBuffer[i];
  }
  filteredGyroZ /= FILTER_SIZE;

  // Detect scroll motion
  if (abs(filteredGyroZ) > SCROLL_THRESHOLD && (millis() - lastScrollTime) > SCROLL_INTERVAL) {
    if (filteredGyroZ > 0) {
      Serial.println("SCROLL_DOWN");
    } else {
      Serial.println("SCROLL_UP");
    }
    lastScrollTime = millis();
  }

  delay(10);
}
