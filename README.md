# 🪐 Arduino Proximars 

[![License: MIT](https://img.shields.io/badge/License-MIT-red.svg?style=for-the-badge&logo=data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAYCAYAAACbU/80AAAC90lEQVRIie2WXW8UVRzGn3N226FNq8RSElCM8TuYeMMX4EbDBR+ICxICpoYEFqk11rS7nZfdLhBuNCEKxHjFpQmGFnUxSI3G1rS7O505Py929iVltt0aDF44yT+Z8/6b//Occ0aRRPAKQ4GEL0skHSoCiRXTL6/K4mfvS4XRxvcAKkYc5klx+BLLxvbqnIMwg3IuPXCOi7Mn/6MALRJIUlrNmJRdAJLEQZKCizuLwVCASAbXjjv9kxSXjQHYgV79+bfeHpIBFxNIRLK0s6pqppfrdtkHwJfwZXoaO9efOhzQPhzqgR5A8QWAXpd9APa6nCEAw034P8CrBAjeO81q1pAHEEncO3Pu0ADpxhPKVlSMyQcIZViYGIdv7pK0/spOtJwM7PzGL+EyC94UwYgAC+MzzJ18B3a2WBmaASuqErv37pMmDM1AE2hU672jdBSAmxKl2VkSYMUeIMHS+6d7DXkAgcSXH545vATPn+5/GYUSNVnir++Caw6XIHU0aj412ZEz4Mvw8bFZ2tnxnQvQjbQewNoTfIl5T/B0HRpr3OwC/Pwjv1+/PjC5oNHpw/pPuQsEEteOvgGNdW4dBND1w2A5lHLN87LiBQB/z4Ju+9d+agfgyrZ/r3fv+EEJfNN/ryp/C+YC7P3iR/PX8CVKxoNnz6gXRNUIGmt89OYJQonHizdoPnzI1VPv4kt8f+ECf353n1WJyAoaG1z1pg/2wF6AUILWFqHE5eOnqBQ8Hl26wiYpkTF8+vpxykakSYuK8XBxkyVjmdc4dYnPxi1pEyIZzh+ZojJW/OcAkZnsy9KGkjxuSCxKkHZ2CrstKipSm/AgjvnktWnS9jYleZQ0Qa0wAkAgMTd5jLnJGS5PzUC83QFJYspj0/wQLOGePyYoHGHj2wedM2QAYLEwwUXvKF/YMb764Cxuc4sVGfhjk9ujSPD5mPBt30R31NnLXdOVbee3qyqxbA2+xK0sW3WJsjHUJMqmSFkey9lci0Wbb8LKv7S9Ro2/AYdeBW+rWK46AAAAAElFTkSuQmCC)](https://mit-license.org/) 


An automated Arduino-based ecosystem monitor and controller. Designed for terrariums or small plant environments, Proximars manages light, watering, humidity, and air quality autonomously.

![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=flat-square&logo=Arduino&logoColor=white)
![Status](https://img.shields.io/badge/Status-Functional-success)

## 🎮 Try it Live!

You can test the logic of this project directly in your browser without any hardware. Click the link below to open the Wokwi simulation:

[![Wokwi Simulation](https://img.shields.io/badge/Wokwi-Test%20Simulation-blue?style=for-the-badge&logo=wokwi)](https://wokwi.com/projects/448491380777419777)

> **Note:** The simulation uses potentiometers to mimic sensors (SGP30, TSL2561) that are not available virtually.

## ✨ Features

* **🌱 Automated Watering:** Activates based on soil moisture levels.
* **💡 Smart Lighting:** Adapts NeoPixel grow lights based on ambient brightness (Lux) and time of day (via Bluetooth).
* **🌡️ Climate Control:** Controls a fan and heater based on temperature thresholds.
* **🌬️ Air Quality Monitoring:** Monitors eCO2 and TVOC levels using the SGP30 sensor, with an audible alarm.
* **📱 Connectivity:** Bluetooth communication for data logging and time synchronization.
* **📊 Display:** Grove RGB LCD showing real-time metrics with color-coded alerts.

## 🛠️ Hardware Requirements

* **Microcontroller:** Arduino Uno or compatible
* **Sensors:**
    * DHT22 (Temperature & Humidity)
    * Adafruit SGP30 (Air Quality)
    * Adafruit TSL2561 (Lux/Brightness)
    * Analog Soil Moisture Sensor
* **Actuators:**
    * NeoPixel LED Strips (x3)
    * Relay Modules (for Heater, Fan, Bubbler, Pump)
    * Buzzer
* **Display:** Grove RGB LCD (I2C)
* **Comms:** HC-05/HC-06 Bluetooth Module

### Mobile App

* You can find the mobile app in the `proximars/proximars.aia` file. To install it, you will need to use the website [MIT App Inventor](https://appinventor.mit.edu/).

*fast tutorial: [MIT App Inventor](https://appinventor.mit.edu/) > Create Apps! > Projects > Import project (.aia) from my computer/repository*

ps: The Arduino needs to be connected via Bluetooth to a cellphone to recover the time and light up the strips when it's the night.

## 📂 Project Structure

The code is modularized for better maintainability:

* `Proximars.ino`: Main entry point and orchestration logic.
* `Config.h`: Pin definitions and configuration constants.
* `Sensors.h`: Handles reading from DHT, SGP30, TSL2561, and Soil sensors.
* `Actuators.h`: Controls Relays, NeoPixels, and Buzzer.
* `Display.h`: Manages the Grove RGB LCD output.
* `Bluetooth.h`: Handles Serial communication.

## 🚀 Installation

1.  Clone this repository.
2.  Install the required libraries via Arduino IDE Library Manager:
    * `DHT sensor library`
    * `Adafruit NeoPixel`
    * `Adafruit SGP30`
    * `Adafruit TSL2561`
    * `Grove - LCD RGB Backlight`
3.  Open `Proximars.ino`.
4.  Upload to your board.

**Thanks**

This project was carried out in collaboration with CNES and participated in the night of researchers at the "Cité de l'Espace" in toulouse . 

## 📄 License

MIT License - Copyright (c) 2024 simonet4