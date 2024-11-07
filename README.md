# Arduino Proximars.

## Hardware.


### obligatory.

  * [Arduino Uno rev3](https://store.arduino.cc/en-fr/products/arduino-uno-rev3/) Or [Arduino Mega 2560 rev3](https://store.arduino.cc/en-fr/products/arduino-mega-2560-rev3/).
  * [RGB backlight positive LCD 16x2](https://www.adafruit.com/product/398).
  * [I2C Temperature and Humidity Sensor](https://www.adafruit.com/product/5183).
  * [I2C Light Sensor Grove](https://wiki.seeedstudio.com/Grove-Digital_Light_Sensor/).
  * [NeoPixel LED Strip - 30 LEDs](https://www.adafruit.com/product/4801).
  * [SGP30 Air Quality Sensor](https://www.adafruit.com/product/3709).
  
### optional, replace with the material of your choice.

  * [Fan](https://www.adafruit.com/product/3368).
  * [Heating](https://www.adafruit.com/product/1481).
  * [Water Pump](https://www.adafruit.com/product/4546) X 2 (watering and Bubbler).
  * [Grove Buzzer](https://www.lextronic.fr/module-grove-buzzer-107020109-58139.html).
  * [bluetooth](https://www.lextronic.fr/module-grove-ble-dual-model-113020009-39435.html).
  * [button](https://www.lextronic.fr/module-bouton-grove-101020003-28884.html).


## Software.

### Libraries.

* `proximars/libraries/Adafruit_BusIO` I2C library from Adafruit.
* `proximars/libraries/Adafruit_NeoPixel` Led strips library from Adafruit.
* `proximars/libraries/Adafruit_SGP30_Sensor` air quality sensor library from Adafruit.
* `proximars/libraries/Adafruit_TSL2561` Luminosity sensor library from Adafruit.
* `proximars/libraries/Adafruit_Unified_Sensor` Temperature and humidity library from Adafruit.
* `proximars/libraries/DHT_sensor_library` Thermocouple library.
* `proximars/libraries/Grove_-_LCD_RGB_Backlight` LCD RGB library.
* `proximars/libraries/Grove_-_LCD_RGB_Backlight` LCD RGB library.

### Mobile App.

* You can find the mobile app in the `proximars/proximars.aia` file. To install it, you will need to use the website [MIT App Inventor](https://appinventor.mit.edu/).

*fast tutorial: [MIT App Inventor](https://appinventor.mit.edu/) > Create Apps! > Projects > Import project (.aia) from my computer/repository*

ps: The Arduino needs to be connected via Bluetooth to a cellphone to recover the time and light up the strips when it's the night.

**Thanks**

This project was carried out in collaboration with CNES and participated in the night of researchers at the "Cité de l'Espace" in toulouse . 