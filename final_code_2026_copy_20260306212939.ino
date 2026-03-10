//IMPORTANT
//Use ESP32 2.0.17 Version Board for Compiling and Uploading
//Made by Darshan Prakash using ChatGPT

#include <Arduino.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// === Pin Definitions ===
#define ENA 25 	 // Right motor PWM
#define IN1 27
#define IN2 26
#define IN3 13
#define IN4 12
#define ENB 32 	 // Left motor PWM

// === PWM Channels ===
#define CH_RIGHT 0
#define CH_LEFT 1

// === PWM Config ===
const int freq = 5000; 	 	// 5 kHz PWM
const int resolution = 8; 	// 8 bits: 0–255 duty cycle

// === Variables ===
int Speed = 250; 	 	 	// Default speed (0–255)
char btSignal;

// === Function Prototypes for new functions ===
void moveForwardLeft();
void moveForwardRight();
void moveBackwardLeft();
void moveBackwardRight();
void stopMotors(); // Prototype added for clarity if stopMotors was used before definition

void setup() {
	Serial.begin(115200);
	SerialBT.begin("Bheema"); 	// Bluetooth name

	// Motor direction pins
	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	pinMode(IN3, OUTPUT);
	pinMode(IN4, OUTPUT);

	// Setup PWM channels
	ledcSetup(CH_RIGHT, freq, resolution);
	ledcAttachPin(ENA, CH_RIGHT);

	ledcSetup(CH_LEFT, freq, resolution);
	ledcAttachPin(ENB, CH_LEFT);

	// Stop motors initially
	stopMotors();
	Serial.println("Bluetooth Car Ready!");
}

// -----------------------------------
// === Motor Control Functions (Existing) ===
// -----------------------------------
void moveForward() {
	digitalWrite(IN1, HIGH);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, HIGH);
	digitalWrite(IN4, LOW);
	ledcWrite(CH_RIGHT, Speed);
	ledcWrite(CH_LEFT, Speed);
}

void moveBackward() {
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, HIGH);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, HIGH);
	ledcWrite(CH_RIGHT, Speed);
	ledcWrite(CH_LEFT, Speed);
}

void turnLeft() {
	// left motor backward, right motor forward (Spin Turn)
	digitalWrite(IN1, HIGH);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, HIGH);
	ledcWrite(CH_RIGHT, Speed);
	ledcWrite(CH_LEFT, Speed);
}

void turnRight() {
	// right motor backward, left motor forward (Spin Turn)
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, HIGH);
	digitalWrite(IN3, HIGH);
	digitalWrite(IN4, LOW);
	ledcWrite(CH_RIGHT, Speed);
	ledcWrite(CH_LEFT, Speed);
}

void stopMotors() {
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, LOW);
	ledcWrite(CH_RIGHT, 0);
	ledcWrite(CH_LEFT, 0);
}

// -----------------------------------
// === New Diagonal Movement Functions ===
// -----------------------------------

/**
 * Moves forward-left by reducing speed on the left motor.
 * This makes the right side drive the car forward more strongly, causing a turn.
 */
void moveForwardLeft() {
	digitalWrite(IN1, HIGH); 	// Right Motor Forward
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, HIGH); 	// Left Motor Forward
	digitalWrite(IN4, LOW);
	ledcWrite(CH_RIGHT, Speed);
	// Slow down the left wheel to make a smooth left turn while moving forward
	ledcWrite(CH_LEFT, Speed / 10); // Use a fractional speed (e.g., half)
}

/**
 * Moves forward-right by reducing speed on the right motor.
 * This makes the left side drive the car forward more strongly, causing a turn.
 */
void moveForwardRight() {
	digitalWrite(IN1, HIGH); 	// Right Motor Forward
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, HIGH); 	// Left Motor Forward
	digitalWrite(IN4, LOW);
	// Slow down the right wheel to make a smooth right turn while moving forward
	ledcWrite(CH_RIGHT, Speed / 5); // Use a fractional speed (e.g., half)
	ledcWrite(CH_LEFT, Speed);
}

/**
 * Moves backward-left by reducing speed on the left motor while reversing.
 */
void moveBackwardLeft() {
	digitalWrite(IN1, LOW); 	// Right Motor Backward
	digitalWrite(IN2, HIGH);
	digitalWrite(IN3, LOW); 	// Left Motor Backward
	digitalWrite(IN4, HIGH);
	ledcWrite(CH_RIGHT, Speed);
	// Slow down the left wheel to make a smooth left turn while moving backward
	ledcWrite(CH_LEFT, Speed / 10); // Use a fractional speed (e.g., half)
}

/**
 * Moves backward-right by reducing speed on the right motor while reversing.
 */
void moveBackwardRight() {
	digitalWrite(IN1, LOW); 	// Right Motor Backward
	digitalWrite(IN2, HIGH);
	digitalWrite(IN3, LOW); 	// Left Motor Backward
	digitalWrite(IN4, HIGH);
	// Slow down the right wheel to make a smooth right turn while moving backward
	ledcWrite(CH_RIGHT, Speed / 5); // Use a fractional speed (e.g., half)
	ledcWrite(CH_LEFT, Speed);
}

// -----------------------------------
// === Main Loop ===
// -----------------------------------
void loop() {
  if (SerialBT.available()) {
    btSignal = SerialBT.read();
    Serial.println(btSignal);

    // Speed control
    if (btSignal >= '0' && btSignal <= '9') {
      Speed = map(btSignal - '0', 0, 9, 100, 255);
    }
    if (btSignal == 'q') Speed = 255;
		// Control logic based on the received character
		switch (btSignal) {
			case 'F': // Forward
				moveForward();
				break;
			case 'B': // Backward
				moveBackward();
				break;
			case 'L': // Spin Left
				turnLeft();
				break;
			case 'R': // Spin Right
				turnRight();
				break;
			case 'S': // Stop
				stopMotors();
				break;

			// --- New Diagonal Movements ---
			case 'I': // Forward Left (usually 'G' in common apps)
				moveForwardLeft();
				break;
			case 'G': // Forward Right (usually 'I' in common apps)
				moveForwardRight();
				break;
			case 'J': // Back Left (usually 'H' in common apps)
				moveBackwardLeft();
				break;
			case 'H': // Back Right (usually 'J' in common apps)
				moveBackwardRight();
				break;

			

			default:
				// Optionally, stop or do nothing for unrecognized commands
				// stopMotors();
				break;
		}
	}
	// Small delay to prevent excessive loop speed, although generally not strictly needed with simple switch logic
	// delay(10);
}