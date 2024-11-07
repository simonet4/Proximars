# Arduino Proximars.

[![License: MIT](https://img.shields.io/badge/License-MIT-red.svg?style=for-the-badge&logo=data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAYCAYAAACbU/80AAAC90lEQVRIie2WXW8UVRzGn3N226FNq8RSElCM8TuYeMMX4EbDBR+ICxICpoYEFqk11rS7nZfdLhBuNCEKxHjFpQmGFnUxSI3G1rS7O505Py929iVltt0aDF44yT+Z8/6b//Occ0aRRPAKQ4GEL0skHSoCiRXTL6/K4mfvS4XRxvcAKkYc5klx+BLLxvbqnIMwg3IuPXCOi7Mn/6MALRJIUlrNmJRdAJLEQZKCizuLwVCASAbXjjv9kxSXjQHYgV79+bfeHpIBFxNIRLK0s6pqppfrdtkHwJfwZXoaO9efOhzQPhzqgR5A8QWAXpd9APa6nCEAw034P8CrBAjeO81q1pAHEEncO3Pu0ADpxhPKVlSMyQcIZViYGIdv7pK0/spOtJwM7PzGL+EyC94UwYgAC+MzzJ18B3a2WBmaASuqErv37pMmDM1AE2hU672jdBSAmxKl2VkSYMUeIMHS+6d7DXkAgcSXH545vATPn+5/GYUSNVnir++Caw6XIHU0aj412ZEz4Mvw8bFZ2tnxnQvQjbQewNoTfIl5T/B0HRpr3OwC/Pwjv1+/PjC5oNHpw/pPuQsEEteOvgGNdW4dBND1w2A5lHLN87LiBQB/z4Ju+9d+agfgyrZ/r3fv+EEJfNN/ryp/C+YC7P3iR/PX8CVKxoNnz6gXRNUIGmt89OYJQonHizdoPnzI1VPv4kt8f+ECf353n1WJyAoaG1z1pg/2wF6AUILWFqHE5eOnqBQ8Hl26wiYpkTF8+vpxykakSYuK8XBxkyVjmdc4dYnPxi1pEyIZzh+ZojJW/OcAkZnsy9KGkjxuSCxKkHZ2CrstKipSm/AgjvnktWnS9jYleZQ0Qa0wAkAgMTd5jLnJGS5PzUC83QFJYspj0/wQLOGePyYoHGHj2wedM2QAYLEwwUXvKF/YMb764Cxuc4sVGfhjk9ujSPD5mPBt30R31NnLXdOVbee3qyqxbA2+xK0sW3WJsjHUJMqmSFkey9lci0Wbb8LKv7S9Ro2/AYdeBW+rWK46AAAAAElFTkSuQmCC)](https://mit-license.org/) 

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
