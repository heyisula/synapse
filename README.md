# SYNAPSE Healthcare Robot 🤖

[![ESP32](https://img.shields.io/badge/ESP32-S3-blue.svg)](https://www.espressif.com/en/products/socs/esp32-s3)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/Build-In%20Progress-yellow.svg)]()

## 📋 Project Overview

The **SYNAPSE Healthcare Robot** is an ESP32-S3 based autonomous assistant designed to enhance healthcare operations in hospital environments. This intelligent mobile platform can follow medical professionals, navigate between departments, transport medical equipment and samples, and monitor patient vital signs.

## 🎯 Key Features

- 🚶‍♂️ **Human Tracking & Following** - Follows doctors and nurses using ultrasonic sensors and color recognition
- 🗺️ **Autonomous Navigation** - Pre-programmed routes between hospital departments
- 🚧 **Obstacle Detection & Avoidance** - 360° obstacle awareness with audio-visual alerts
- ❤️ **Patient Vital Signs Monitoring** - Heart rate and SpO2 measurement capabilities
- 🌡️ **Environmental Monitoring** - Temperature, humidity, and light level tracking
- 🎨 **Color-Based Staff Recognition** - Distinguishes between doctors and nurses by uniform colors
- 📱 **Real-time Communication** - WiFi status updates and Bluetooth control
- 🔋 **Long Battery Life** - 4-6 hours of continuous operation

## 🛠️ Hardware Components

### Core Processing
- **ESP32-S3 Development Board N16R8** - Main controller with WiFi/Bluetooth
- **4 Wheel Smart Car Chassis** - Structural foundation

### Sensors
- **HC-SR04 Ultrasonic Sensors (4x)** - Human detection and obstacle avoidance
- **MAX30102** - Heart rate and pulse oximetry sensor
- **DHT22** - Temperature and humidity sensor
- **TCS3200** - Color recognition sensor for staff identification
- **MPU-6050** - 6-axis gyroscope and accelerometer
- **KY-040 Rotary Encoders (2x)** - Wheel rotation tracking
- **LDR Light Sensors (4x)** - Ambient light monitoring
- **5-Bit Line Following IR Sensor** - Path navigation

### Actuators & Output
- **14V 200RPM Gear Motors (4x)** - High-torque movement system
- **L298N Motor Drivers (2x)** - Motor control modules
- **16x4 LCD Display** - Status and information display
- **Piezo Buzzers (2x)** - Audio alerts and notifications
- **Emergency Stop Button** - Safety control

### Power Management
- **12V 3500mAh Li-Po Battery** - Main power source
- **CA-1235 Buck Converter** - Voltage regulation (1.25V-5V)
- **DC Charging Jack** - Battery charging interface

## 🎯 Core Functions

### 1. Human Tracking & Following
- Detects and follows medical professionals using ultrasonic distance measurement
- Color sensor identifies staff roles (doctors vs. nurses) based on uniform colors
- Maintains safe following distance during procedures and patient rounds

### 2. Autonomous Navigation & Delivery
- Navigates between predefined locations (doctor's office ↔ laboratory ↔ pharmacy)
- Delivers medical samples, test results, medications, and documents
- Uses line-following sensors and motion tracking for precise navigation

### 3. Obstacle Detection & Avoidance
- 360° obstacle awareness using multiple ultrasonic sensors
- Automatic stopping with audio-visual alerts when obstacles detected
- Request obstacle removal before continuing mission

### 4. Vital Signs Monitoring
- Non-invasive heart rate and SpO2 monitoring using MAX30102 sensor
- Real-time patient status tracking with emergency alert generation
- Continuous monitoring during medical procedures

### 5. Environmental Monitoring
- Tracks temperature, humidity, and lighting conditions
- Ensures optimal conditions for medical equipment and patient comfort
- Maintains environmental data logs for compliance purposes

### 6. Color-Based Staff Recognition
- Identifies medical equipment and supplies using color coding systems
- Distinguishes between medical professionals by uniform colors
- Prioritizes interactions based on staff hierarchy (doctors > nurses)

## 📁 Project Structure

```
synapse/
├── src/
│   ├── main.cpp                 # Main program entry point
│   ├── sensors/
│   │   ├── ultrasonic.h         # HC-SR04 sensor functions
│   │   ├── vital_signs.h        # MAX30102 heart rate sensor
│   │   ├── color_sensor.h       # TCS3200 color recognition
│   │   ├── environmental.h      # DHT22 temp/humidity
│   │   └── navigation.h         # MPU-6050 and encoders
│   ├── actuators/
│   │   ├── motors.h             # Motor control functions
│   │   ├── display.h            # LCD display management
│   │   └── alerts.h             # Buzzer and LED controls
│   ├── communication/
│   │   ├── wifi_manager.h       # WiFi connectivity
│   │   └── bluetooth.h          # Bluetooth communication
│   └── utils/
│       ├── config.h             # Configuration constants
│       ├── state_machine.h      # Robot state management
│       └── safety.h             # Emergency stop functions
├── libraries/                   # External libraries
├── docs/                        # Documentation
├── hardware/                    # Circuit diagrams and PCB files
├── 3d_models/                   # 3D printable parts
├── README.md                    # This file
├── LICENSE                      # License file
└── platformio.ini              # PlatformIO configuration
```

## 🚀 Getting Started

### Prerequisites
- [PlatformIO](https://platformio.org/) or [Arduino IDE](https://www.arduino.cc/en/software)
- ESP32-S3 board support
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
   - Update `src/utils/config.h` with your WiFi credentials
   - Adjust sensor calibration values if needed

4. **Build and upload**
   ```bash
   pio run --target upload
   ```

### Hardware Setup

1. **Assemble the chassis** and mount all sensors according to the hardware documentation
2. **Connect sensors** following the wiring diagram in `/docs/wiring_diagram.png`
3. **Calibrate sensors** using the provided calibration sketches
4. **Test individual components** before full integration

## 📖 Usage

### Basic Operation Modes

**1. Following Mode**
- Robot follows detected medical personnel
- Maintains 1-2 meter safe distance
- Responds to color-coded uniform identification

**2. Delivery Mode**  
- Autonomous navigation between preset locations
- Follows marked paths using line sensors
- Secure transport of medical items

**3. Monitoring Mode**
- Stationary vital signs monitoring
- Environmental condition tracking
- Real-time data display and alerts

**4. Emergency Mode**
- Immediate stop on obstacle detection
- Emergency button activation
- Alert broadcasting to medical staff

### Control Interface

- **LCD Display**: Shows current status, sensor readings, and alerts
- **Emergency Button**: Immediate stop functionality
- **WiFi Interface**: Remote monitoring and control
- **Bluetooth**: Emergency override and configuration

## ⚕️ Safety Features

- **Emergency stop button** for immediate shutdown
- **Collision detection** with automatic braking
- **Speed limiting** in different hospital zones
- **Battery monitoring** with low-power warnings
- **Fail-safe modes** for sensor failures
- **Visual/audio alerts** for obstacle detection

## 🔧 Configuration

### Sensor Calibration
- Ultrasonic sensor range: 20cm - 4m
- Color sensor: RGB threshold values for uniform recognition
- Heart rate sensor: Baseline calibration for accurate readings
- Temperature/humidity: Environmental baseline settings

### Navigation Settings
- Following distance: 1.0 - 2.0 meters (adjustable)
- Maximum speed: 1.0 m/s (hospital safe speed)
- Turn radius: Minimum 0.5m for corridor navigation
- Line following sensitivity: Adjustable IR threshold

## 📊 Performance Specifications

| Parameter | Value |
|-----------|--------|
| Operating Time | 4-6 hours continuous |
| Maximum Speed | 1.0 m/s |
| Detection Range | 20cm - 4m |
| Following Accuracy | ±10cm |
| Navigation Precision | ±5cm |
| Response Time | <100ms |
| Weight Capacity | 5kg medical equipment |

## 🤝 Contributing

We welcome contributions to improve the SYNAPSE Healthcare Robot! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Development Guidelines
- Follow the existing code style and naming conventions
- Add comments for complex algorithms
- Test thoroughly on hardware before submitting
- Update documentation for new features

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- ESP32 community for excellent documentation and libraries
- Healthcare professionals who provided requirements feedback  
- Open source sensor library contributors
- University supervisors and project mentors

## 📞 Contact & Support

- **Email**: [isulacodes@gmail.com]
- **Issues**: [GitHub Issues](https://github.com/heyisula/synapse/issues)
- **Documentation**: [Project Wiki](https://github.com/heyisula/synapse/wiki)

## 🔮 Future Enhancements

- [ ] Machine learning for improved staff recognition
- [ ] Integration with hospital information systems
- [ ] Voice command interface
- [ ] Advanced pathfinding algorithms
- [ ] Multi-robot coordination
- [ ] Cloud-based monitoring dashboard
- [ ] Mobile app for remote control
- [ ] Medication dispensing capability

---

**⚠️ Disclaimer**: This is a prototype developed for educational purposes. Not intended for actual medical use without proper certification and regulatory approval.

**🏥 Healthcare Compliance**: Ensure compliance with local healthcare regulations and safety standards before deployment in medical facilities.
