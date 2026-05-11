# 🎮 ESP8266 OLED Retro Game Console

A mini retro gaming console built using **ESP8266 (NodeMCU)** and **0.96" SSD1306 OLED Display**.  
This project includes multiple classic games, menu navigation, password unlock system, and bitmap-based graphics.

---

## 🚀 Features

- 📟 SSD1306 OLED Display Support
- 🎮 13 Built-in Retro Games
- 🔐 Secret Password Unlock Screen
- 🕹️ 5 Button Navigation System
- ⚡ Smooth Bitmap Animations
- 📈 Score System
- 👾 Retro Arcade Feel

---

## 🕹️ Included Games

| No. | Game |
|------|------|
| 1 | Dino Run |
| 2 | Snake |
| 3 | Flappy Stone |
| 4 | Tic Tac Toe |
| 5 | Pong |
| 6 | Mario Mini |
| 7 | Car Dodger |
| 8 | Tappy Tap |
| 9 | Space Invaders |
| 10 | Tetris Lite |
| 11 | Brick Killer |
| 12 | 2048 |
| 13 | Doodle Jump |

---

## 🧰 Hardware Required

- ESP8266 NodeMCU
- SSD1306 OLED Display (128x64 I2C)
- 5 Push Buttons
- Breadboard / PCB
- Jumper Wires
- Battery (Optional)

---

## 🔌 Pin Connections

### OLED Display

| OLED | ESP8266 |
|------|----------|
| VCC | 3.3V |
| GND | GND |
| SDA | D2 |
| SCL | D1 |

### Buttons

| Button | Pin |
|--------|-----|
| UP | D6 |
| DOWN | D7 |
| OK | D5 |
| LEFT | D3 |
| RIGHT | D4 |

---

## 📚 Libraries Used

Install these libraries from Arduino Library Manager:

- Adafruit GFX
- Adafruit SSD1306

---

## 🔑 Secret Unlock

The console starts with a password lock.

### Password:
```text
UP → UP → UP → UP



🖼️ Graphics

All game sprites are custom bitmap graphics designed for monochrome OLED displays.

Includes:

Dino
Mario
Space Ships
Cars
Goombas
Pipes
Invaders
Bricks
Doodler


⚙️ How to Upload
Install Arduino IDE
Install ESP8266 Board Package
Select:
Board: NodeMCU 1.0 (ESP-12E Module)
Install required libraries
Upload the code


👨‍💻 Author

Made with ❤️ using ESP8266 and OLED Display.

GitHub: naresh
