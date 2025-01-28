// Libraries
#include <Wire.h> // Include Wire library for I2C communication
#include <rgb_lcd.h> // Include RGB LCD library for controlling the LCD
#include <DHT.h> // Include DHT library for temperature and humidity sensor
#include <Adafruit_Sensor.h> // Include Adafruit Sensor library
#include <Adafruit_TSL2561_U.h> // Include TSL2561 light sensor library
#include <SoftwareSerial.h> // Include SoftwareSerial library for Bluetooth communication
#include <Adafruit_NeoPixel.h> // Include NeoPixel library for controlling LED strips
#include <Adafruit_SGP30.h> // Include SGP30 air quality sensor library
#include "Proximars_Void.h" 

//License
// MIT License
// 
// Copyright (c) 2024 https://github.com/simonet4
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


// Pins on the Arduino
// Digital pins
#define ButtonPin A3 // Button connected to pin A3
#define BubblerPin 2 // Bubble maker connected to pin 2
#define wateringPin 3 // Watering system connected to pin 3
#define heatPin 4 // Heating element connected to pin 4
#define FanPin 5 // Fan connected to pin 5
#define buzzerPin 6 // Buzzer connected to pin 6
#define led1 11 // Data pin for the first part of the LED strip
#define led2 12 // Data pin for the second part of the LED strip
#define led3 13 // Data pin for the third part of the LED strip
#define NUM_LEDS 30 // Number of LEDs in each part of the strip
SoftwareSerial BTserial(7, 8); // Bluetooth module RX on pin 7, TX on pin 8

// Analog pins
DHT dht(A0, DHT22); // DHT sensor for temperature and humidity on pin A0

// I2C devices
Adafruit_TSL2561_Unified tsl(0x29); // TSL2561 light sensor on I2C address 0x29
rgb_lcd lcd; // RGB LCD display object
Adafruit_SGP30 sgp; // SGP30 air quality sensor object

// Variables
unsigned long actButton = 0; // Last time the button was activated
unsigned long acthum = 0; // Last time humidity was checked
unsigned long switchAutoButton = 0; // Last time the automatic button switch occurred
unsigned long lastsendBluetooth = 0; // Last time data was sent via Bluetooth
unsigned long UP = 0; // Last time sensor data was updated
unsigned long Desact = 0; // Last time the system was deactivated
unsigned long interUP = 10; // Sensor update interval (10 milliseconds)
unsigned long intersendBluetooth = 300; // Bluetooth sending interval (300 milliseconds)
unsigned long AFK = 10000; // 10 seconds of inactivity
unsigned long interswitchAutoButton = 3000; // 3 seconds interval for automatic toggle
unsigned long AFKMin = 21600000; // Minimum inactivity duration after watering (6 hours)
unsigned long actwatering = 300000; // Duration for watering (5 minutes)

bool actcurrent = false; // Current activation state

int typedatacurrent = 0; // Current data type to display (0: temp, 1: air humidity, 2: soil humidity, 3: brightness)
int displayMode = 0; // Initial display mode (0 for temperature)
int statePrecedentButton = LOW; // Previous state of the button

// Custom degree symbol for LCD
byte DEGREE_SYMBOL = 0; 
byte degree[8] = {
    0b00000,
    0b00010,
    0b00101,
    0b00010,
    0b00000,
    0b00000,
    0b00000,
    0b00000
};

// Initialize LED strips
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(NUM_LEDS, led1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(NUM_LEDS, led2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(NUM_LEDS, led3, NEO_GRB + NEO_KHZ800);

void setup() {
  // Initializations
  Serial.begin(9600); // Initialize serial communication for debugging
  BTserial.begin(9600); // Initialize Bluetooth serial communication
  lcd.begin(16, 2); // Initialize the LCD screen (2 lines, 16 characters)
  lcd.print("Hello, world!"); // Display initial message on the LCD
  lcd.createChar(DEGREE_SYMBOL, degree); // Create custom degree symbol on the LCD
  dht.begin(); // Initialize the DHT sensor
    if (!tsl.begin()) { // Initialize the TSL sensor (brightness) and check for success
    Serial.println("TSL sensor not found :("); // Print error message if sensor not found
  }
  if (!sgp.begin()) { // Initialize the SGP sensor (air quality) and check for success
    Serial.println("SGP sensor not found :("); // Print error message if sensor not found
  }
  tsl.enableAutoRange(true); // Enable auto range for brightness measurements
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS); // Set short integration time for quick measurements
  strip1.begin(); // Initialize the first LED strip
  strip2.begin(); // Initialize the second LED strip
  strip3.begin(); // Initialize the third LED strip

  // Set pin modes for inputs and outputs
  pinMode(ButtonPin, INPUT); // Set button pin as input
  pinMode(BubblerPin, OUTPUT); // Set bubble maker pin as output
  pinMode(wateringPin, OUTPUT); // Set watering pin as output
  pinMode(heatPin, OUTPUT); // Set heating pin as output
  pinMode(FanPin, OUTPUT); // Set fan pin as output

  // Initialize all LEDs to "off" (black) for each part of the strip
  strip1.show();
  strip2.show();
  strip3.show();
}

void loop() {
  unsigned long now = millis(); // Get the current time in milliseconds

  // Air quality measurement and alarm
  if (!sgp.IAQmeasure()) { // Measure air quality
    Serial.println("Measurement failed"); // Print error message if measurement fails
    return; // Exit the function if measurement fails
  }
  // Print air quality values to the serial monitor
  Serial.print("TVOC "); Serial.print(sgp.TVOC); Serial.print(" ppb\t");
  Serial.print("eCO2 "); Serial.print(sgp.eCO2); Serial.println(" ppm");

  // Activate buzzer if air quality thresholds are exceeded
  if (sgp.eCO2 >= 1500 || sgp.TVOC >= 500) { // Check if eCO2 or TVOC exceeds thresholds
    tone(buzzerPin, 3000, 500); // Activate buzzer for 500 ms at 3000 Hz
  }

  // Read the current state of the button
  int stateButton = digitalRead(ButtonPin); // Read button state

  // Bluetooth communication to receive current hour
  if (BTserial.available()) { // Check if data is available from Bluetooth
    int hour = BTserial.read(); // Read the hour from Bluetooth
    Serial.print("Received hour: ");
    Serial.println(hour); // Print received hour for debugging

    // Control the LEDs based on the received hour
    if (hour >= 8 && hour <= 20) { // If the hour is between 8 AM and 8 PM
      float light = readlight(); // Read current light level
      if (light < 500) { // If light level is below threshold
        // Turn on the LEDs in purple
        allumerLED(strip1, 128, 0, 128);
        allumerLED(strip2, 128, 0, 128);
        allumerLED(strip3, 128, 0, 128);
      } else {
        // Turn off the LEDs
        eteindreLED(strip1);
        eteindreLED(strip2);
        eteindreLED(strip3);
      }
    } else {
      // Turn off the LEDs if outside the hour range
      eteindreLED(strip1);
      eteindreLED(strip2);
      eteindreLED(strip3);
    }
  }

  // Button action for automatic display interval
  if (stateButton == HIGH) { // If button is pressed
    displayMode = (displayMode + 1) % 5; // Change display mode on each button press
    actButton = now; // Update the last activation time
  }
  statePrecedentButton = stateButton; // Store the current button state

  unsigned long timepass = now - actButton; // Calculate time since last button activation
  if (timepass >= AFK) { // If button has been inactive for 10 seconds
    if (now - switchAutoButton >= interswitchAutoButton) { // If 3 seconds have passed since last automatic toggle
      displayMode = (displayMode + 1) % 4; // Change display mode automatically
      switchAutoButton = now; // Update the last automatic toggle time
    }
  }

  // Control components and display data on LCD/Bluetooth via sensors
  if (now - UP >= interUP) { // If it's time to update sensor data
    float humsoil = readhumsoil(); // Read soil humidity
    float light = readlight(); // Read light level
    float temp = dht.readTemperature(); // Read temperature
    UP = now; // Update the last update time

    // Control components based on sensor readings
    // Control the bubble maker and watering based on soil humidity
    if (humsoil < 60 && !actcurrent) { // If soil humidity is below 60% and not currently active
      ActivateBubbler(HIGH); // Activate the bubble maker
      Activatewatering(HIGH); // Activate watering
      acthum = now; // Update the last activation time for humidity
      actcurrent = true; // Set current activation state to true
    }
    if (actcurrent && now - acthum >= actwatering) { // If currently active and watering duration has passed
      ActivateBubbler(LOW); // Deactivate the bubble maker
      Activatewatering(LOW); // Deactivate watering
      Desact = now; // Update the last deactivation time
      actcurrent = false; // Set current activation state to false
    }
    if (!actcurrent && now - Desact >= AFKMin) { // If not currently active and minimum inactivity duration has passed
      acthum = 0; // Reset humidity activation time
      Desact = 0; // Reset deactivation time
    }

    // Control the heating element based on temperature
    if (temp < 5) { // If temperature is below 5°C
      digitalWrite(heatPin, HIGH); // Turn on heating
    } else {
      digitalWrite(heatPin, LOW); // Turn off heating
    }

    // Control the fan based on temperature
    if (temp > 25) { // If temperature is above 25°C
      digitalWrite(FanPin, HIGH); // Turn on fan
    } else {
      digitalWrite(FanPin, LOW); // Turn off fan
    }

    // LCD and Bluetooth display
    if (now - lastsendBluetooth >= intersendBluetooth) { // If it's time to send data via Bluetooth
      envoyerdataBluetooth(typedatacurrent); // Send current data type via Bluetooth
      typedatacurrent = (typedatacurrent + 1) % 4; // Move to the next data type
      lastsendBluetooth = now; // Update the last sending time
    }

    // Display data on the LCD based on the current display mode
    switch (displayMode) {
      case 0: // Display temperature
        showtemp(); // Call function to show temperature
        break;
      case 1: // Display air humidity
        showhumAir(); // Call function to show air humidity
        break;
      case 2: // Display soil humidity
        showhumsoil(); // Call function to show soil humidity
        break;
      case 3: // Display brightness
        showlight(); // Call function to show brightness
        break;
    }
  }
}