#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <SoftwareSerial.h>
#include "Config.h"

class BluetoothManager {
private:
    SoftwareSerial BTserial;

public:
    BluetoothManager() : BTserial(PIN_BT_RX, PIN_BT_TX) {}

    void begin() {
        BTserial.begin(9600);
    }

    int readHour() {
        if (BTserial.available()) {
            int h = BTserial.read();
            Serial.print("BT Hour received: "); Serial.println(h);
            return h;
        }
        return -1; // No data
    }

    void sendData(int type, float temp, float hum, float soil, float light, int co2) {
        String label = "";
        float value = 0;

        switch (type) {
            case 0: label = "T "; value = temp; break;
            case 1: label = "Ha "; value = hum; break;
            case 2: label = "Hs "; value = soil; break;
            case 3: label = "L "; value = light; break;
            case 4: label = "Q "; value = co2; break;
        }
        
        String msg = label + String(value);
        BTserial.println(msg);
        Serial.println("BT Sent: " + msg);
    }
};

#endif