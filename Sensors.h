#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <Adafruit_SGP30.h>
#include "Config.h"

class SensorManager {
private:
    DHT dht;
    Adafruit_TSL2561_Unified tsl;
    Adafruit_SGP30 sgp;
    
    float temp;
    float humAir;
    float humSoil;
    float light;
    
public:
    SensorManager() : dht(PIN_DHT, DHT22), tsl(0x29) {}

    void begin() {
        dht.begin();
        
        if (!tsl.begin()) {
            Serial.println("Error: TSL2561 sensor not found!");
        } else {
            tsl.enableAutoRange(true);
            tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
        }

        if (!sgp.begin()) {
            Serial.println("Error: SGP30 sensor not found!");
        }
    }

    void update() {
        // Read DHT
        float t = dht.readTemperature();
        float h = dht.readHumidity();
        if (!isnan(t)) temp = t;
        if (!isnan(h)) humAir = h;

        // Read Soil
        int val = analogRead(PIN_SOIL_SENSOR);
        humSoil = map(val, 0, 700, 0, 100); // Calibration based on original code

        // Read Light
        sensors_event_t event;
        tsl.getEvent(&event);
        if (event.light) {
            light = event.light;
        }

        // Read Air Quality (SGP30)
        if (!sgp.IAQmeasure()) {
            Serial.println("SGP30 Measurement failed");
        }
    }

    // Getters
    float getTemp() { return temp; }
    float getHumAir() { return humAir; }
    float getHumSoil() { return humSoil; }
    float getLight() { return light; }
    uint16_t getTVOC() { return sgp.TVOC; }
    uint16_t geteCO2() { return sgp.eCO2; }
};

#endif