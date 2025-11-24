/*
 * Proximars - Automated Ecosystem Controller
 * Original Author: simonet4
 * Refactored for modularity
 */

#include "Config.h"
#include "Sensors.h"
#include "Actuators.h"
#include "Display.h"
#include "Bluetooth.h"

// Objects
SensorManager sensors;
ActuatorManager actuators;
DisplayManager display;
BluetoothManager bt;

// State Variables
unsigned long now = 0;
unsigned long lastSensorUpdate = 0;
unsigned long lastBTSend = 0;
unsigned long lastButtonPress = 0;
unsigned long lastAutoSwitch = 0;

// Watering States
unsigned long wateringStartTime = 0;
unsigned long lastWateringTime = 0; // Desact in original
bool isWatering = false;

// Display States
int displayMode = 0;
int btDataType = 0;
int currentHour = -1; // Stores hour received from BT

void setup() {
  Serial.begin(9600);
  
  sensors.begin();
  actuators.begin();
  display.begin();
  bt.begin();
  
  pinMode(PIN_BUTTON, INPUT);
}

void loop() {
  now = millis();

  // --- 1. SENSOR & LOGIC UPDATE ---
  if (now - lastSensorUpdate >= SENSOR_UPDATE_INTERVAL) {
    lastSensorUpdate = now;
    sensors.update();

    // CO2 Alarm
    if (sensors.geteCO2() >= 1500 || sensors.getTVOC() >= 500) {
      actuators.triggerAlarm();
    }

    // Watering Logic
    // If dry (<60), not currently watering, and enough time passed since last watering (6h)
    if (sensors.getHumSoil() < 60 && !isWatering) {
        if (lastWateringTime == 0 || (now - lastWateringTime >= WATERING_COOLDOWN)) {
             actuators.setBubbler(HIGH);
             actuators.setWatering(HIGH);
             wateringStartTime = now;
             isWatering = true;
        }
    }
    
    // Stop Watering
    if (isWatering && (now - wateringStartTime >= WATERING_DURATION)) {
      actuators.setBubbler(LOW);
      actuators.setWatering(LOW);
      lastWateringTime = now;
      isWatering = false;
    }

    // Climate Control
    actuators.setHeater(sensors.getTemp() < 5);
    actuators.setFan(sensors.getTemp() > 25);

    // Light Logic (Based on BT Hour and Lux)
    // If hour is valid (received via BT) and between 8am-8pm
    if (currentHour >= 8 && currentHour <= 20) {
        if (sensors.getLight() < 500) {
            actuators.setAllLeds(128, 0, 128); // Purple
        } else {
            actuators.setAllLeds(0, 0, 0); // Off
        }
    } else if (currentHour != -1) {
        // Night time
        actuators.setAllLeds(0, 0, 0);
    }
    
    // Update Display
    display.updateScreen(displayMode, sensors.getTemp(), sensors.getHumAir(), sensors.getHumSoil(), sensors.getLight(), sensors.geteCO2());
  }

  // --- 2. BLUETOOTH ---
  // Read Incoming Hour
  int h = bt.readHour();
  if (h != -1) {
      currentHour = h;
  }

  // Send Data periodically
  if (now - lastBTSend >= BT_SEND_INTERVAL) {
      bt.sendData(btDataType, sensors.getTemp(), sensors.getHumAir(), sensors.getHumSoil(), sensors.getLight(), sensors.geteCO2());
      btDataType = (btDataType + 1) % 5;
      lastBTSend = now;
  }

  // --- 3. USER INPUT (BUTTON) ---
  int buttonState = digitalRead(PIN_BUTTON);
  if (buttonState == HIGH) { // Assuming Active High based on original code
     if (now - lastButtonPress > 200) {
         displayMode = (displayMode + 1) % 5;
         lastButtonPress = now;
         lastAutoSwitch = now; // Reset AFK timer
     }
  }

  // Auto-switch display if inactive
  if (now - lastButtonPress >= AFK_TIMEOUT) {
      if (now - lastAutoSwitch >= AUTO_SWITCH_INTERVAL) {
          displayMode = (displayMode + 1) % 4;
          lastAutoSwitch = now;
      }
  }
}