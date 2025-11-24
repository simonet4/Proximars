#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "Config.h"

class ActuatorManager {
private:
    Adafruit_NeoPixel strip1;
    Adafruit_NeoPixel strip2;
    Adafruit_NeoPixel strip3;

public:
    ActuatorManager() : 
        strip1(NUM_LEDS, PIN_LED_STRIP_1, NEO_GRB + NEO_KHZ800),
        strip2(NUM_LEDS, PIN_LED_STRIP_2, NEO_GRB + NEO_KHZ800),
        strip3(NUM_LEDS, PIN_LED_STRIP_3, NEO_GRB + NEO_KHZ800) {}

    void begin() {
        pinMode(PIN_BUBBLER, OUTPUT);
        pinMode(PIN_WATERING, OUTPUT);
        pinMode(PIN_HEATER, OUTPUT);
        pinMode(PIN_FAN, OUTPUT);
        pinMode(PIN_BUZZER, OUTPUT);
        
        strip1.begin(); strip1.show();
        strip2.begin(); strip2.show();
        strip3.begin(); strip3.show();
    }

    void setBubbler(bool state) { digitalWrite(PIN_BUBBLER, state); }
    void setWatering(bool state) { digitalWrite(PIN_WATERING, state); }
    void setHeater(bool state) { digitalWrite(PIN_HEATER, state); }
    void setFan(bool state) { digitalWrite(PIN_FAN, state); }
    
    void triggerAlarm() { tone(PIN_BUZZER, 3000, 500); }

    void setAllLeds(uint8_t r, uint8_t g, uint8_t b) {
        setStripColor(strip1, r, g, b);
        setStripColor(strip2, r, g, b);
        setStripColor(strip3, r, g, b);
    }

private:
    void setStripColor(Adafruit_NeoPixel &strip, uint8_t r, uint8_t g, uint8_t b) {
        for(int i=0; i< strip.numPixels(); i++) {
            strip.setPixelColor(i, strip.Color(r, g, b));
        }
        strip.show();
    }
};

#endif