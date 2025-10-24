# 💧 Water Level Monitor (Dual Pump System - ESP32 + Blynk)

This project monitors the water level in a tank using an **HC-SR04 ultrasonic sensor**, displays it on a **16x2 I2C LCD**, and controls two pumps:
- **Pump 1**: Automatic operation (with Blynk override)
- **Pump 2**: Manual switch operation (local control)

---

## 🚀 Features

- 📶 Wi-Fi setup using **WiFiManager** (no hardcoded credentials)
- 🌊 Real-time water level monitoring (% display)
- 💡 LCD + Blynk dashboard integration
- ⚙️ Auto Pump Control:
  - Turns ON below 10%
  - Turns OFF above 95%
- 🔘 Manual override via Blynk (V0)
- 🕹️ Separate manual pump using local switch

---

## 🧠 System Overview

| Component | Function |
|------------|-----------|
| **ESP32** | Controls and monitors system |
| **HC-SR04** | Measures water level distance |
| **LCD (I2C)** | Displays level and system status |
| **Motor Driver (L298N/L293D)** | Drives two water pumps |
| **Blynk App** | Monitors level and controls Pump 1 |
| **Switch** | Controls Pump 2 manually |

---

## 🔌 Circuit Connections

| Component | ESP32 Pin | Description |
|------------|------------|-------------|
| Ultrasonic TRIG | 18 | Trigger pin |
| Ultrasonic ECHO | 19 | Echo pin |
| LCD SDA | 21 | I2C SDA |
| LCD SCL | 22 | I2C SCL |
| Motor1 IN1 | 14 | Pump1 control |
| Motor1 IN2 | 12 | Pump1 control |
| Motor2 IN3 | 13 | Pump2 control |
| Motor2 IN4 | 27 | Pump2 control |
| Pump2 Switch | 26 | Local manual switch |
| Power | 5V & GND | Common ground required |

---

## 📲 Blynk Dashboard Setup

| Virtual Pin | Widget | Function |
|--------------|---------|-----------|
| V0 | Button | Manual ON/OFF (Pump 1) |
| V1 | Gauge / Label | Water Level % |

---

## 🧰 Required Libraries

- **WiFiManager**
- **Blynk**
- **LiquidCrystal_I2C**

Install via Arduino Library Manager.

---

## ⚙️ How It Works

1. ESP32 starts and opens Wi-Fi configuration access point (“**WaterLevel**”).
2. User connects and configures Wi-Fi credentials.
3. Ultrasonic sensor measures distance continuously.
4. Water level percentage displayed on LCD and Blynk.
5. Automatic pump control logic:
   - Below 10% → Pump1 ON
   - Above 95% → Pump1 OFF
6. Manual pump (Pump2) operates via local switch.

---

## 🪛 Future Enhancements

- Add water level history graph in Blynk
- Add overflow/buzzer alert
- Integrate temperature & humidity sensor

---

## 📜 License

This project is licensed under the **MIT License**.

---
