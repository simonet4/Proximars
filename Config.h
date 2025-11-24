#ifndef CONFIG_H
#define CONFIG_H

// --- PINS DEFINITIONS ---
// Digital
#define PIN_BUBBLER 2
#define PIN_WATERING 3
#define PIN_HEATER 4
#define PIN_FAN 5
#define PIN_BUZZER 6
#define PIN_BT_RX 7
#define PIN_BT_TX 8
// LED Strips Data Pins
#define PIN_LED_STRIP_1 11
#define PIN_LED_STRIP_2 12
#define PIN_LED_STRIP_3 13

// Analog
#define PIN_DHT A0
#define PIN_SOIL_SENSOR A2
#define PIN_BUTTON A3

// --- CONFIGURATION ---
#define NUM_LEDS 30
#define SENSOR_UPDATE_INTERVAL 2000  // ms (Standard for DHT/SGP)
#define BT_SEND_INTERVAL 3000        // ms
#define AFK_TIMEOUT 10000            // ms
#define AUTO_SWITCH_INTERVAL 3000    // ms
#define WATERING_DURATION 300000     // 5 minutes
#define WATERING_COOLDOWN 21600000   // 6 hours

#endif