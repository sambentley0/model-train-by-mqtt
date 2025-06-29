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
- Secure MQTT connection (username/password)
- OTA-friendly and modular

---

## ⚙️ Hardware Components

- ESP32 development board  
- DRV8871 motor driver  
- Buck converter (12V to 5V)  
- 1000uF 25V capacitor  
- Bridge rectifier (MB6S, KBP206 or 4x 1N5819 diodes)  
- 12V DC from track

---

## 🛠️ Wiring Instructions

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
   - ESP32 3.3V → DRV8871 SLP (or controlled via GPIO)

7. **DRV8871 Motor Output**
   - OUT+ → Motor terminal A
   - OUT- → Motor terminal B

---

### Notes

- Always use a common ground between ESP32, buck converter, and DRV8871  
- The bridge rectifier guarantees polarity protection if the train is rotated on the track  
- Capacitor smoothing improves performance over dirty rail sections  
- 25V capacitor is recommended for voltage headroom  
- Use appropriate wire gauges and strain relief

---

## 🛰️ MQTT Broker Setup (Raspberry Pi)

### 1️⃣ Install Mosquitto

sudo apt update
sudo apt install mosquitto mosquitto-clients

Enable and start it:

sudo systemctl enable mosquitto
sudo systemctl start mosquitto

---

### 2️⃣ Add a user for secure connections

Create a new user (for example, trainuser) with a password:

sudo mosquitto_passwd -c /etc/mosquitto/passwd trainuser

---

### 3️⃣ Configure Mosquitto

Edit the Mosquitto config file:

sudo nano /etc/mosquitto/mosquitto.conf

Add these lines to the bottom:

allow_anonymous false
password_file /etc/mosquitto/passwd
listener 1883

Restart Mosquitto to apply changes:

sudo systemctl restart mosquitto

---

### 4️⃣ Test the broker

Open two terminals on the Pi (or any PC with Mosquitto clients installed):

Terminal 1 (subscribe):

mosquitto_sub -h localhost -u trainuser -P yourpassword -t "test/topic"

Terminal 2 (publish):

mosquitto_pub -h localhost -u trainuser -P yourpassword -t "test/topic" -m "hello world"

✅ You should see 'hello world' appear in Terminal 1.

---

### 5️⃣ Configure the ESP32

Add to secrets.h on the ESP32:

const char* mqtt_user = "trainuser";
const char* mqtt_pass = "yourpassword";

And in your sketch’s reconnect() function, use:

if (client.connect(train_id, mqtt_user, mqtt_pass)) {

✅ This will securely connect to your Pi’s broker with username/password protection.

---

## 🛰️ MQTT Topics

Each train uses a unique short code based on its ESP32 MAC address, for example train/7F4D/.

| Purpose            | Topic                                     |
|--------------------|-------------------------------------------|
| Speed command      | train/7F4D/speed                          |
| Direction command  | train/7F4D/direction                      |
| Status reporting   | train/7F4D/status                         |
| Name announce      | train/7F4D/name                           |
| Config friendlyname| train/7F4D/config/friendlyname            |
| Config maxspeed    | train/7F4D/config/maxspeed                |
| Config accelrate   | train/7F4D/config/accelrate               |

---

## 🖥️ Dashboard

You can control this system using:

- Node-RED
- Home Assistant
- MQTT Dashboard apps
- Any MQTT client

Node-RED is recommended for an easy visual dashboard with sliders, buttons, and text fields.

---

## 📂 Project Structure

ESP32_Train_Controller/
  |-- ESP32_Train_Controller.ino
  |-- secrets_template.h
  |-- README.md
  |-- LICENSE
  |-- .gitignore

✅ Add secrets.h locally (with your real credentials) and be sure it is listed in .gitignore.

---

## 🚀 Getting Started

1. Wire your circuit as described above  
2. Upload the code using Arduino IDE or PlatformIO  
3. Copy secrets_template.h to secrets.h and edit with your Wi-Fi and MQTT details  
4. Fire up Node-RED or Home Assistant to send MQTT commands  
5. Enjoy wireless model railway control!

---

## 📝 License

MIT License — see LICENSE for details.

---

Happy railroading! 🚂
