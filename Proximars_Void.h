#ifndef PROXIMARS_VOID_H
#define PROXIMARS_VOID_H

// Function to send data via Bluetooth based on the current data type
void envoyerdataBluetooth(int typedata);

// Function to display temperature on the LCD
void showtemp();

// Function to turn on all LEDs in a specified color
void allumerLED(Adafruit_NeoPixel &strip, uint8_t r, uint8_t g, uint8_t b);

// Function to turn off all LEDs
void eteindreLED(Adafruit_NeoPixel &strip);

// Function to display air humidity on the LCD
void showhumAir();

// Function to display soil humidity on the LCD
void showhumsoil();

// Function to read soil humidity from the analog sensor
float readhumsoil();

// Function to display brightness on the LCD
void showlight();

// Function to read brightness from the TSL2561 light sensor
float readlight();

// Function to activate the bubble maker
void ActivateBubbler(int SpeedBubbler);

// Function to activate the watering system
void Activatewatering(int Speedwatering);

#endif