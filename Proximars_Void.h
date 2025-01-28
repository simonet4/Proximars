#include <Wire.h> // Include Wire library for I2C communication
#include <rgb_lcd.h> // Include RGB LCD library for controlling the LCD
#include <DHT.h> // Include DHT library for temperature and humidity sensor
#include <Adafruit_Sensor.h> // Include Adafruit Sensor library
#include <Adafruit_TSL2561_U.h> // Include TSL2561 light sensor library
#include <SoftwareSerial.h> // Include SoftwareSerial library for Bluetooth communication
#include <Adafruit_NeoPixel.h> // Include NeoPixel library for controlling LED strips
#include <Adafruit_SGP30.h> // Include SGP30 air quality sensor library

// Function to send data via Bluetooth based on the current data type
void envoyerdataBluetooth(int typedata) {
  float value = 0; // Variable to hold the value to send
  String text = ""; // Variable to hold the label for the data type

  // Select the data type to send based on the counter
  switch (typedata) {
    case 0: // Temperature
      value = dht.readTemperature(); // Read temperature
      text = "T "; // Label for temperature
      break;
    case 1: // Air humidity
      value = dht.readHumidity(); // Read air humidity
      text = "Ha "; // Label for air humidity
      break;
    case 2: // Soil humidity
      value = readhumsoil(); // Read soil humidity
      text = "Hs "; // Label for soil humidity
      break;
    case 3: // Brightness
      value = readlight(); // Read brightness
      text = "L "; // Label for brightness
      break;
    case 4: // Air quality
      value = sgp.eCO2; // Read eCO2 value
      text = "Q "; // Label for air quality
      break;
    default:
      break;
  }

  // Build the message to send
  String message = text + String(value); // Create message string

  // Send the message via Bluetooth
  BTserial.println(message); // Send the message

  // Display the sent message on the serial console (for debugging)
  Serial.print("Sending Bluetooth data: ");
  Serial.println(message); // Print the message for debugging
}

// Function to display temperature on the LCD
void showtemp() {
  float temp = dht.readTemperature(); // Read temperature
  lcd.clear(); // Clear the LCD display
  lcd.setCursor(0, 0); // Set cursor to the first line
  lcd.print("temp:"); // Print "temp:" on the first line
  lcd.setCursor(0, 1); // Set cursor to the second line
  lcd.print(temp); // Print the temperature value
  lcd.write(DEGREE_SYMBOL); // Print the degree symbol
  lcd.print("C"); // Print "C" for Celsius
  Serial.print("temp: "); // Print to serial for debugging
  Serial.print(temp); // Print the temperature value
  Serial.println("C"); // Print "C" for Celsius

  // Define temperature ranges and corresponding colors
  const float coldest = 18.0; // Coldest temperature (blue)
  const float hottest = 35.0; // Hottest temperature (red)

  // Calculate color based on temperature
  int red = map(temp, coldest, hottest, 0, 255); // Map temperature to red value
  int green = 0; // No green for this example
  int blue = map(temp, coldest, hottest, 255, 0); // Map temperature to blue value

  // Limit RGB values to valid range (0-255)
  red = constrain(red, 0, 255); // Constrain red value
  blue = constrain(blue, 0, 255); // Constrain blue value

  // Apply color to the LCD screen
  lcd.setRGB(red, green, blue); // Set the RGB color of the LCD
}

// Function to turn on all LEDs in a specified color
void allumerLED(Adafruit_NeoPixel &strip, uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_LEDS; i++) { // Loop through each LED
    strip.setPixelColor(i, strip.Color(r, g, b)); // Set the color of the LED
  }
  strip.show(); // Update the LED strip to show the new colors
}

// Function to turn off all LEDs
void eteindreLED(Adafruit_NeoPixel &strip) {
  for (int i = 0; i < NUM_LEDS; i++) { // Loop through each LED
    strip.setPixelColor(i, strip.Color(0, 0, 0)); // Set the color to off (black)
  }
  strip.show(); // Update the LED strip to show the new colors
}

// Function to display air humidity on the LCD
void showhumAir() {
  float humair = dht.readHumidity(); // Read air humidity
  lcd.clear(); // Clear the LCD display
  lcd.setCursor(0, 0); // Set cursor to the first line
  lcd.print("Humidity Air:"); // Print "Humidity Air:" on the first line
  lcd.setCursor(0, 1); // Set cursor to the second line
  lcd.print(humair); // Print the humidity value
  lcd.print("%"); // Print "%" for percentage
  Serial.print("Humidity Air: "); // Print to serial for debugging
  Serial.print(humair); // Print the humidity value
  Serial.println("%"); // Print "%" for percentage
}

// Function to display soil humidity on the LCD
void showhumsoil() {
  float humsoil = readhumsoil(); // Read soil humidity
  lcd.clear(); // Clear the LCD display
  lcd.setCursor(0, 0); // Set cursor to the first line
  lcd.print("Humidity Soil:"); // Print "Humidity Soil:" on the first line
  lcd.setCursor(0, 1); // Set cursor to the second line
  lcd.print(humsoil); // Print the humidity value
  lcd.print("%"); // Print "%" for percentage
  Serial.print("Humidity Soil: "); // Print to serial for debugging
  Serial.print(humsoil); // Print the humidity value
  Serial.println("%"); // Print "%" for percentage
}

// Function to read soil humidity from the analog sensor
float readhumsoil() {
  int valuesensor = analogRead(A2); // Read the analog value from the soil moisture sensor
  float humsoil = map(valuesensor, 0, 700, 0, 100); // Map the sensor value to a percentage (0-100)
  return humsoil; // Return the soil humidity value
}

// Function to display brightness on the LCD
void showlight() {
  float light = readlight(); // Read brightness level
  lcd.clear(); // Clear the LCD display
  lcd.setCursor(0, 0); // Set cursor to the first line
  lcd.print("Brightness:"); // Print "Brightness:" on the first line
  lcd.setCursor(0, 1); // Set cursor to the second line
  lcd.print(light); // Print the brightness value
  lcd.print(" lux"); // Print "lux" for lux units
  Serial.print("Brightness: "); // Print to serial for debugging
  Serial.print(light); // Print the brightness value
  Serial.println(" lux"); // Print "lux" for lux units
}

// Function to read brightness from the TSL2561 light sensor
float readlight() {
  sensors_event_t event; // Create an event object to hold sensor data
  tsl.getEvent(&event); // Get the light event data from the sensor
  if (event.light) { // Check if the light value is valid
    return event.light; // Return the light value
  } else {
    return -1.0; // Return -1.0 if the read fails (invalid value)
  }
}

// Function to activate the bubble maker
void ActivateBubbler(int SpeedBubbler) {
  digitalWrite(BubblerPin, SpeedBubbler); // Set the bubble maker pin to the specified state (HIGH or LOW)
}

// Function to activate the watering system
void Activatewatering(int Speedwatering) { 
  digitalWrite(wateringPin, Speedwatering); // Set the watering pin to the specified state (HIGH or LOW)
}
