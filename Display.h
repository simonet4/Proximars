#ifndef DISPLAY_H
#define DISPLAY_H

#include <Wire.h>
#include <rgb_lcd.h> // Library for Grove RGB LCD
#include "Config.h"

class DisplayManager {
private:
    rgb_lcd lcd;
    byte degreeSymbol[8] = {0b00010,0b00101,0b00010,0,0,0,0,0}; // Custom char

public:
    void begin() {
        lcd.begin(16, 2);
        lcd.createChar(0, degreeSymbol);
        lcd.print("Proximars Init");
    }

    void updateScreen(int mode, float temp, float humAir, float humSoil, float light, int eco2) {
        // Change background color based on Temp (Blue to Red)
        // Simple mapping: 18°C = Blue, 35°C = Red
        int red = map(temp, 18, 35, 0, 255);
        int blue = map(temp, 18, 35, 255, 0);
        lcd.setRGB(constrain(red, 0, 255), 0, constrain(blue, 0, 255));

        lcd.clear();
        lcd.setCursor(0,0);
        
        switch(mode) {
            case 0: // Temp
               lcd.print("Temp: "); lcd.print(temp); lcd.write((unsigned char)0); lcd.print("C");
               break;
            case 1: // Air Humidity
               lcd.print("Air Hum: "); lcd.print(humAir); lcd.print("%");
               break;
            case 2: // Soil Humidity
               lcd.print("Soil Hum: "); lcd.print(humSoil); lcd.print("%");
               break;
            case 3: // Light
               lcd.print("Light: "); lcd.print(light); lcd.print(" lx");
               break;
             case 4: // CO2 (Added for better monitoring)
               lcd.print("CO2: "); lcd.print(eco2); lcd.print(" ppm");
               break;
        }
    }
};

#endif