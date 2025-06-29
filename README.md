# 🚂 ESP32 WiFi Train Controller

This project is an ESP32-based controller for OO gauge model trains, using MQTT for fully wireless operation. The system drives a DC motor through a DRV8871 H-bridge motor driver, powered by track-supplied 12V DC.  

---

## ✨ Features

- WiFi-controlled over MQTT
- Speed control (0–255)
- Direction control (forward / reverse / stop)
- Configurable max speed
- Configurable acceleration ramp
- Friendly train name with storage in flash
- Bridge rectifier for polarity protection
- Status reporting (speed and direction)
- OTA-friendly and modular

---

## ⚙️ Hardware Components

- ESP32 development board  
- DRV8871 motor driver  
- Buck converter (12V to 5V)  
- 1000uF 25V capacitor  
- Bridge rectifier (MB6S, KBP206 or 4x 1N5819 diodes)  
- Your OO gauge DC motor and 12V power on rails

---

## 🛠️ Wiring Instructions

This guide shows how to wire the system to get correct polarity, power filtering, and robust motor drive.

### **Wiring Steps**

1. **Track Power to Bridge Rectifier**
   - Left Rail → Bridge AC input (~)
   - Right Rail → Bridge AC input (~)

2. **Bridge Rectifier to Capacitor**
   - Rectifier `+` → Capacitor `+` (long leg)
   - Rectifier `-` → Capacitor `-` (short leg)

3. **Capacitor Output to Buck Converter**
   - Capacitor `+` → Buck VIN+
   - Capacitor `-` → Buck GND

4. **Buck Converter to ESP32**
   - Buck VOUT+ (5V) → ESP32 VIN
   - Buck GND → ESP32 GND

5. **Capacitor Output to DRV8871 Power**
   - Capacitor `+` → DRV8871 VM
   - Capacitor `-` → DRV8871 GND

6. **ESP32 to DRV8871 Control Pins**
   - GPIO 16 → DRV8871 IN1 (PWM)
   - GPIO 17 → DRV8871 IN2
   - ESP32 GND → DRV8871 GND
   - ESP32 3.3V → DRV8871 SLP (or control via GPIO)

7. **DRV8871 Motor Output**
   - OUT+ → Motor terminal A
   - OUT- → Motor terminal B

---

### **Notes**

- Always use a common ground between ESP32, buck converter, and DRV8871  
- The bridge rectifier guarantees polarity protection if the train is rotated on the track  
- Capacitor smoothing improves performance over dirty rail sections  
- 25V capacitor is recommended for headroom  
- Use appropriate wire gauges and strain relief

---

## 🛰️ MQTT Topics
Each train uses a unique short code based on its ESP32 MAC address, for example `train/7F4D/`.  

| Purpose            | Topic                                     |
|--------------------|-------------------------------------------|
| Speed command      | `train/7F4D/speed`                        |
| Direction command  | `train/7F4D/direction`                    |
| Status reporting   | `train/7F4D/status`                       |
| Name announce      | `train/7F4D/name`                         |
| Config friendlyname| `train/7F4D/config/friendlyname`           |
| Config maxspeed    | `train/7F4D/config/maxspeed`               |
| Config accelrate   | `train/7F4D/config/accelrate`              |

---

## 🖥️ Dashboard

You can control this system from:

- Node-RED
- Home Assistant
- MQTT Dashboard app
- Any MQTT-compatible client

A sample Node-RED flow can provide:

- a speed slider  
- direction buttons  
- friendly name input  
- numeric fields for max speed and acceleration  
- live status text
