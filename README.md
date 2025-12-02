# SYNAPSE (Healthcare Robot)

[![ESP32](https://img.shields.io/badge/ESP32-S3%20%2B%20WROOM32-blue.svg)](https://www.espressif.com/en/products/socs/esp32-s3)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/Build-In%20Progress-yellow.svg)]()
[![Progress](https://img.shields.io/badge/Progress-75%25-orange.svg)]()

## 📋 Project Overview

**SYNAPSE** is an autonomous healthcare assistant robot designed to enhance operational efficiency in hospital environments. Built with a dual ESP32 architecture (ESP32-S3 + ESP32-WROOM32), this intelligent mobile platform automates routine tasks like following medical staff, navigating between departments, transporting medical equipment and samples, and monitoring patient vital signs.

### 🎯 Project Goals

- **Reduce manual transportation workload by 60-70%**
- **Achieve 99% operational uptime**
- **Enable 1 hour of continuous operation per charge**
- **Maintain navigation precision within ±5cm**
- **Provide intuitive physical controls** for gloved-hand operation

## ✨ Key Features

### 🚶‍♂️ Human Tracking & Following
- Follows medical professionals using ultrasonic sensors (1-2m safe distance)
- Color-based staff recognition (doctors vs. nurses by uniform)
- Priority-based following system

### 🗺️ Autonomous Navigation & Delivery
- Pre-programmed routes between hospital departments
- Line-following with 5-bit IR sensor array
- Precision waypoint navigation with MPU-6050 IMU

### 🚧 Obstacle Detection & Avoidance
- 360° obstacle awareness with 4x HC-SR04 ultrasonic sensors
- Real-time collision prevention (<1 second response time)
- Audio-visual alerts via buzzers and LED arrays

### ❤️ Patient Vital Signs Monitoring
- Heart rate and SpO2 measurement (MAX30102 sensor)
- ±2% accuracy compared to medical-grade devices
- Real-time display with emergency alerts

### 🌡️ Environmental Monitoring
- Temperature, humidity, and ambient light tracking
- Storage compartment security monitoring (LDR sensors)
- Optimal conditions for medical equipment transport

### 🎨 Color-Based Staff Recognition
- TCS3200 RGB color sensor
- Distinguishes staff roles by uniform colors
- Restricted zone detection (red-marked areas)

### 🎛️ Intuitive User Interface
- 16x4 LCD display with real-time status
- KY-040 rotary encoder for gloved-hand menu control
- Emergency stop button for immediate safety shutdown
- Standalone operation (no smartphone/computer needed)

## 🏗️ System Architecture

### Dual Processing Architecture

**ESP32-S3 (Main Controller)**
- Navigation and sensor processing
- User interface management
- WiFi/Bluetooth communication
- I2C bus master (LCD, MPU-6050, MAX30102)

**ESP32-WROOM32 (Motor Controller)**
- Dedicated real-time motor control
- Emergency stop handling
- Movement command execution
- Reduced latency for motor operations

**Benefits:**
- Parallel processing (sensors + motors)
- Enhanced reliability with redundancy
- Modular design for easier maintenance
- Optimal resource utilization

## 🛠️ Hardware Components

### Core Processing
- **ESP32-S3 Development Board N16R8** - 240MHz dual-core, 16MB Flash, 8MB PSRAM
- **ESP32-WROOM32 Development Board** - 240MHz dual-core, 4MB Flash

### Sensors
- **HC-SR04 Ultrasonic Sensors (4x)** - 360° obstacle detection (2-100cm range)
- **5-Bit Line Following IR Sensor** - Path navigation with ±2cm accuracy
- **MPU-6050 IMU** - 6-axis gyroscope/accelerometer for orientation tracking
- **TCS3200 Color Sensor** - RGB staff recognition and zone detection
- **MAX30102 Pulse Oximeter** - Heart rate and SpO2 monitoring
- **AM2302 Temperature/Humidity Sensor** - Environmental monitoring (±0.5°C accuracy)
- **LDR Light Sensors (4x)** - Path visibility and compartment security

### Actuators & Output
- **4x Mecanum Wheels** - Omnidirectional movement capability
- **14V 200RPM Gear Motors (4x)** - High-torque movement system
- **L298N Motor Drivers (2x)** - Independent motor control
- **16x4 LCD Display** - Real-time status with I2C interface
- **KY-040 Rotary Encoder (2x)** - Menu control and adjustments
- **Piezo Buzzers (2x)** - Directional audio alerts
- **LED Arrays** - Visual status indicators

### Power Management
- **12V 3500mAh Li-Po Battery** - Main power source
- **CA-1235 Buck Converter** - 12V to 5V regulation
- Emergency stop with hardware interrupt

## 📁 Project Structure
```
SYNAPSE-Healthcare-Robot/
├── ESP32-S3-Main/              # Main Controller Firmware
│   ├── src/
│   │   ├── main.cpp
│   │   ├── config/             # Pin definitions & constants
│   │   ├── sensors/            # Sensor interfaces
│   │   ├── actuators/          # Display, buzzer, LED control
│   │   ├── ui/                 # Menu system & rotary encoder
│   │   ├── modes/              # Operation modes
│   │   ├── navigation/         # Path planning & tracking
│   │   ├── communication/      # UART, WiFi, Bluetooth
│   │   ├── safety/             # Collision & emergency handling
│   │   └── utils/              # State machine, filters, PID
│   └── lib/                    # External libraries
│
├── ESP32-WROOM-Motor/          # Motor Controller Firmware
│   ├── src/
│   │   ├── main.cpp
│   │   ├── motor_control/      # L298N drivers & movement
│   │   ├── communication/      # UART handler
│   │   └── safety/             # E-stop & motor protection
│   └── test/
│
├── docs/                       # Documentation & guides
│   ├── flowcharts/             # System flowcharts
│   ├── wiring_guide.md
│   └── api_reference.md
│
└── hardware/                   # Circuit diagrams & 3D models
    ├── circuit_diagrams/
    ├── pcb_designs/
    └── 3d_models/
```

## 🚀 Getting Started

### Prerequisites
- [PlatformIO](https://platformio.org/) or [Arduino IDE](https://www.arduino.cc/en/software)
- ESP32-S3 and ESP32-WROOM32 board support
- Required libraries (see `platformio.ini`)

### Installation

1. **Clone the repository**
```bash
   git clone https://github.com/heyisula/synapse.git
   cd synapse
```

2. **Install dependencies**
```bash
   pio lib install
```

3. **Configure settings**
   - Update `src/config/pins.h` with your pin configuration
   - Adjust sensor thresholds in `src/config/thresholds.h`
   - Configure WiFi credentials in `src/communication/wifi_manager.h`

4. **Build and upload**
```bash
   # Upload to ESP32-S3 Main Controller
   cd ESP32-S3-Main
   pio run --target upload
   
   # Upload to ESP32-WROOM32 Motor Controller
   cd ../ESP32-WROOM-Motor
   pio run --target upload
```

### Hardware Setup
1. Assemble the 4-wheel Mecanum chassis
2. Mount all sensors according to the wiring diagram
3. Connect ESP32-S3 and ESP32-WROOM32 via UART (TX/RX pins)
4. Calibrate sensors using test sketches
5. Test individual components before full integration

## 📖 Usage

### Operation Modes

**1. Following Mode**
- Tracks and follows medical personnel
- Maintains 1-2 meter safe distance
- Color-based staff identification

**2. Delivery Mode**
- Autonomous navigation between preset locations
- Line-following with junction detection
- Secure transport of medical items

**3. Monitoring Mode**
- Patient vital signs tracking
- Environmental condition monitoring
- Real-time alerts for abnormal readings

**4. Emergency Mode**
- Immediate stop on button press or obstacle detection
- Alert broadcasting via buzzers
- Status display on LCD

### Menu Control (KY-040 Rotary Encoder)
- **Rotate:** Navigate menu options
- **Press:** Confirm selection
- **Long Press:** Return to main menu

**Adjustable Parameters:**
- Following distance: 0.5m - 2.0m
- Robot speed: 20% - 100%
- Display brightness: 0% - 100%
- Buzzer volume: 0-10 levels
- Auto mode: ON/OFF

## ⚙️ Communication Protocols

### I2C Bus (ESP32-S3)
- **LCD Display:** 0x27
- **MPU-6050 IMU:** 0x68
- **MAX30102 Pulse Oximeter:** 0x57

### UART (Inter-ESP Communication)
- **Baud Rate:** 115200 bps
- **ESP32-S3 TX (GPIO 43) → ESP32-WROOM32 RX (GPIO 0)**
- **ESP32-S3 RX (GPIO 44) → ESP32-WROOM32 TX (GPIO 3)**

## 📊 Performance Specifications

| Parameter | Target | Current Status |
|-----------|--------|----------------|
| Operating Time | 4 hours | 1-1.5 hours |
| Maximum Speed | 0.5 m/s | Testing |
| Detection Range | 20-100 cm | ✅ Verified |
| Navigation Precision | ±5 cm | Testing |
| Emergency Stop Response | <1 second | Testing |
| Sensor Accuracy | ±2% | Testing |

## 🔧 Configuration

### Pin Assignments

**ESP32-S3 Main Controller:**
- Ultrasonic sensors: GPIO 4-11
- I2C Bus: GPIO 2 (SDA), GPIO 3 (SCL)
- LDR sensors: GPIO 15-18
- KY-040 encoder: GPIO 38-40
- UART: GPIO 43 (TX), GPIO 44 (RX)

**ESP32-WROOM32 Motor Controller:**
- L298N motor drivers: GPIO 12-23
- Emergency button: GPIO 27
- UART: GPIO 0 (RX), GPIO 3 (TX)

## 🛡️ Safety Features

- **Hardware emergency stop** with direct motor controller connection
- **360° collision detection** with automatic braking
- **Real-time health monitoring** of all subsystems
- **Low battery warnings** with automatic shutdown
- **Fail-safe modes** for sensor failures
- **Audio-visual alerts** for obstacle detection

## 🤝 Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Development Guidelines
- Follow existing code style and naming conventions
- Add comments for complex algorithms
- Test thoroughly on hardware before submitting
- Update documentation for new features

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 📞 Contact

- **Email:** isulacodes@gmail.com
- **GitHub Issues:** [Report a bug or request a feature](https://github.com/heyisula/synapse/issues)

## 🔮 Future Enhancements

- [ ] Machine learning for improved staff recognition
- [ ] Integration with hospital information systems
- [ ] Voice command interface
- [ ] Advanced pathfinding with SLAM
- [ ] Multi-robot coordination
- [ ] Cloud-based monitoring dashboard
- [ ] Mobile app for remote control

---

**⚠️ Disclaimer:** This is an educational prototype. Not intended for actual medical use without proper certification and regulatory approval.

**🏥 Healthcare Compliance:** Ensure compliance with local healthcare regulations and safety standards before deployment in medical facilities.
