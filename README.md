# SYNAPSE: Automating Excellence in Healthcare

<div align="center">

![ESP32](https://img.shields.io/badge/ESP32-S3%20%2B%20WROOM32-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)
![Progress](https://img.shields.io/badge/Progress-Complete-brightgreen.svg)
![IoT](https://img.shields.io/badge/IoT-Healthcare-orange.svg)

**An autonomous healthcare assistant robot designed to enhance operational efficiency in hospital environments**

[Live Demo](https://heyisula.github.io/synapse/) • [Documentation](https://github.com/heyisula/synapse/tree/main/docs) • [Hardware Designs](https://github.com/heyisula/synapse/tree/main/hardware)

</div>

---

## 📋 Table of Contents

- [About SYNAPSE](#about-synapse)
- [Key Features](#key-features)
- [System Architecture](#system-architecture)
- [Hardware Components](#hardware-components)
- [Software Stack](#software-stack)
- [Getting Started](#getting-started)
- [Usage Guide](#usage-guide)
- [Project Structure](#project-structure)
- [Performance Metrics](#performance-metrics)
- [Web Interface](#web-interface)
- [Contributing](#contributing)
- [License](#license)

---

## 🧠 About SYNAPSE

### The Meaning Behind "SYNAPSE"

Just as biological synapses enable seamless communication in the nervous system, SYNAPSE creates vital connections in the hospital environment:

- **Rapid Response Communication**: Real-time data transfer between sensors, controllers, and healthcare systems
- **Intelligent Information Relay**: Filters and prioritizes critical patient data to reduce information overload
- **Network Integration**: Connects various hospital departments, staff members, and monitoring systems
- **Bridging Gaps**: Spans operational gaps between medical staff, patients, departments, and technology
- **Adaptive Learning**: Logs data and can be updated to improve performance over time

### Why This Robot Is Needed

Modern healthcare facilities face critical operational challenges:

- **Staff Time Optimization**: Medical professionals spend 2-3 hours daily on routine transportation tasks (25-30% reduction in direct patient care time)
- **Resource Efficiency**: Highly skilled professionals should focus on clinical activities rather than logistics
- **Safety and Reliability**: Manual processes risk sample contamination, equipment misplacement, and procedural delays
- **Continuous Monitoring**: Current systems have gaps during procedures or between check-ups

**SYNAPSE addresses these challenges with:**
- 60-70% reduction in manual transportation workload
- 99% operational uptime with 100% collision avoidance capability
- Continuous patient monitoring with ±2% accuracy
- 2+ hours continuous operation per charge cycle

---

## ✨ Key Features

### 🚶‍♂️ Staff Identification and Human Following
- **Color-Based Recognition**: TCS3200 sensor identifies staff by uniform color (white coats vs. colored scrubs)
- **Priority-Based Following**: Doctors > nurses based on uniform identification
- **Safe Distance Maintenance**: 1.0-2.0m optimal following distance with ultrasonic sensors
- **Adaptive Speed**: Matches staff member's walking pace automatically
- **Hands-Free Operation**: Carries medical supplies during patient rounds

### 🗺️ Autonomous Navigation
- **Obstacle Avoidance**: 360° coverage with 4× HC-SR04 ultrasonic sensors (2-100cm range)
- **Line Following**: 5-bit IR sensor array for precise path navigation (±2cm accuracy)
- **Emergency Stop**: <1 second response time with hardware interrupt
- **Orientation Tracking**: MPU6050 IMU for stable tracking and positioning
- **McNamum Wheels**: Omnidirectional mobility for complex hospital corridors

### ❤️ Patient Vital Signs Monitoring
- **Heart Rate & SpO2**: MAX30102 pulse oximeter with ±2% accuracy
- **Real-Time Display**: 1-second update intervals on 16×4 LCD
- **Emergency Alerts**: Automatic notifications for abnormal readings
- **Medical-Grade Accuracy**: Meets healthcare monitoring standards

### 🌡️ Environmental Monitoring
- **Temperature & Humidity**: AM2302 sensor (±0.5°C, ±3% RH accuracy)
- **Ambient Light Monitoring**: 4× LDR sensors for path visibility and compartment security
- **Optimal Transport Conditions**: Ensures proper storage for medical samples
- **Adaptive Lighting**: Automatic LED control based on ambient conditions

### 🎛️ Intuitive User Interface
- **16×4 LCD Display**: Real-time status, sensor readings, and alerts
- **KY-040 Rotary Encoder**: Gloved-hand operable menu control
- **Standalone Operation**: No smartphone/computer dependency
- **Emergency Push Button**: Immediate safety shutdown
- **Audio-Visual Alerts**: Dual piezo buzzers with directional placement

### 🌐 IoT Integration
- **Web-Based Control**: Remote monitoring and control interface
- **Real-Time Data Sync**: Cloud connectivity via Firebase
- **Google Analytics**: Usage tracking and performance monitoring
- **Mobile Optimized**: Responsive design for smartphones and tablets
- **Secure Authentication**: Role-based access control

---

## 🏗️ System Architecture

### Dual ESP32 Processing Architecture

SYNAPSE employs an innovative dual-microcontroller design for enhanced reliability and performance:

#### **ESP32-S3 (Main Controller)**
- **Specifications**: 240MHz dual-core, 16MB Flash, 8MB PSRAM
- **Role**: Navigation, sensor processing, user interface management
- **Responsibilities**:
  - All sensor data acquisition and processing
  - User interface control (LCD, KY-040 encoder)
  - WiFi/Bluetooth communication for IoT features
  - Decision-making algorithms
  - Command generation for motor controller

#### **ESP32 WROOM-32 (Motor Controller)**
- **Specifications**: 240MHz dual-core, 4MB Flash
- **Role**: Dedicated motor control and mechanical operations
- **Responsibilities**:
  - Real-time motor speed and direction control
  - Emergency stop system (hardware interrupt)
  - Execution of movement commands
  - PWM signal generation for 4 motors

#### **Advantages of Dual Architecture**
1. **Parallel Processing**: Sensor processing doesn't interfere with time-critical motor control
2. **Enhanced Reliability**: If one controller fails, the system operates in degraded mode
3. **Reduced Latency**: Immediate motor response without waiting for sensor processing
4. **Modularity**: Easier troubleshooting and independent firmware updates
5. **Load Distribution**: Optimal capacity utilization for each controller

### Inter-Controller Communication
- **Protocol**: UART serial communication
- **Baud Rate**: 9600 bps
- **Connections**:
  - ESP32-S3 TX (GPIO 43) → ESP32 WROOM-32 RX (GPIO 0)
  - ESP32-S3 RX (GPIO 44) ← ESP32 WROOM-32 TX (GPIO 3)
  - Common ground for stable communication

---

## 🛠️ Hardware Components

### Core Processing
| Component | Specifications |
|-----------|---------------|
| ESP32-S3 Development Board N16R8 | 240MHz dual-core, 16MB Flash, 8MB PSRAM |
| ESP32 WROOM-32 Development Board | 240MHz dual-core, 4MB Flash |

### Navigation & Safety Sensors
| Sensor | Quantity | Specifications | Purpose |
|--------|----------|----------------|---------|
| HC-SR04 Ultrasonic | 4 | 2-100cm range, <1s response | 360° obstacle detection, human following |
| MPU6050 IMU | 1 | 6-axis gyro/accelerometer | Orientation tracking, tilt detection |
| 5-Bit IR Array | 1 | 2-10mm detection range | Line following (±2cm accuracy) |
| TCS3200 Color Sensor | 1 | RGB detection, 16.7M colors | Staff uniform identification |
| LDR Modules | 4 | 50-1000 Lux detection | Path visibility, security monitoring |

### Patient Monitoring Sensors
| Sensor | Specifications | Purpose |
|--------|----------------|---------|
| MAX30102 Pulse Oximeter | ±2% accuracy, 1s updates | Heart rate and SpO2 monitoring |
| AM2302 Temperature/Humidity | ±0.5°C temp, ±3% RH | Environmental monitoring |

### Motor Control System
| Component | Quantity | Specifications |
|-----------|----------|----------------|
| McNamum Wheels | 4 | 60mm diameter | Omnidirectional mobility |
| Gear Motors | 4 | 24V 200RPM 25GA-370 | High-torque movement |
| L298N Motor Drivers | 2 | Dual H-bridge | Independent motor control |

### Display & Interface
| Component | Specifications |
|-----------|----------------|
| 16×4 LCD Display | I2C interface (0x27) |
| KY-040 Rotary Encoder | 2 units for menu control |
| Piezo Buzzers | 2 units for stereo alerts |
| LED Arrays | 4× 1W high-power LEDs |

### Power Management
| Component | Specifications |
|-----------|----------------|
| 12V 3500mAh Li-Po Battery | Primary power source |
| CA-1235 Buck Converter | 12V to 5V regulation |

---

## 💻 Software Stack

### Development Environment
- **Platform**: PlatformIO / Arduino IDE
- **Language**: C++ (ESP32 Arduino Framework)
- **Version Control**: Git/GitHub

### Communication Protocols
- **I2C Bus**: LCD Display (0x27), MPU-6050 (0x68), MAX30102 (0x57)
- **UART**: Inter-ESP communication (9600 bps)
- **WiFi**: 802.11 b/g/n for IoT connectivity
- **Bluetooth**: 5.0 LE / 4.2 for local control

### Libraries and Dependencies

```ini
lib_deps =
    marcoschwartz/LiquidCrystal_I2C@^1.1.4
    sparkfun/SparkFun MAX3010x Pulse and Proximity Sensor Library@^1.1.2
    adafruit/Adafruit Unified Sensor@^1.1.15
    hasenradball/AM2302-Sensor@^1.4.0
    panjkrc/tcs3200@^1.3.1
    powerbroker2/SerialTransfer@^3.1.5
    mobizt/Firebase ESP32 Client@^4.4.17
    electroniccats/MPU6050@^1.4.4
    teckel12/NewPing@^1.9.7
    denyssene/SimpleKalmanFilter@^0.1.0
```

### Key Library Functions
- **LiquidCrystal_I2C**: Simple I2C interface for LCD display
- **SparkFun MAX3010x**: Accurate heart rate and SpO2 measurement
- **AM2302-Sensor**: Improved reliability over standard DHT libraries
- **SerialTransfer**: Reliable data packets with CRC error checking
- **Firebase ESP32 Client**: Real-time database synchronization
- **NewPing**: Ultrasonic sensor accuracy with ping median filtering
- **SimpleKalmanFilter**: Mathematical filtering for stable sensor readings

---

## 🚀 Getting Started

### Prerequisites
- [PlatformIO](https://platformio.org/) or [Arduino IDE](https://www.arduino.cc/en/software)
- ESP32-S3 and ESP32-WROOM32 board support
- USB-C cable for programming
- Git for version control

### Installation

1. **Clone the repository**
```bash
git clone https://github.com/heyisula/synapse.git
cd synapse
```

2. **Install PlatformIO** (recommended)
```bash
# Install PlatformIO Core
pip install platformio

# Or use PlatformIO IDE extension for VS Code
```

3. **Install dependencies**
```bash
# PlatformIO will automatically install libraries from platformio.ini
pio lib install
```

### Hardware Setup

1. **Assemble the chassis**
   - Mount McNamum wheels on gear motors
   - Attach motors to chassis frame
   - Install L298N motor drivers

2. **Mount sensors**
   - Position 4× ultrasonic sensors for 360° coverage
   - Install IR line sensor array at bottom
   - Mount MPU6050 IMU centrally
   - Attach color sensor at front
   - Place LDR sensors for path and compartment monitoring

3. **Connect ESP32 boards**
   - Wire ESP32-S3 according to pin assignments
   - Wire ESP32-WROOM32 for motor control
   - Connect UART communication (TX/RX crossover)
   - Ensure common ground between boards

4. **Power system**
   - Connect 12V battery to buck converter
   - Wire 5V output to ESP32 boards
   - Connect motor drivers to 12V rail

5. **User interface**
   - Connect 16×4 LCD via I2C
   - Wire KY-040 rotary encoders
   - Install emergency stop button
   - Connect buzzers and LEDs

### Firmware Upload

#### ESP32-S3 Main Controller
```bash
cd ESP32-S3-Main
pio run --target upload
pio device monitor  # Optional: view serial output
```

#### ESP32-WROOM32 Motor Controller
```bash
cd ESP32-WROOM-Motor
pio run --target upload
pio device monitor  # Optional: view serial output
```

### Initial Configuration

1. **WiFi Setup**
   - Update WiFi credentials in `src/config/wifi_config.h`
   - Configure Firebase credentials for IoT features

2. **Sensor Calibration**
   - Run calibration sketches for each sensor type
   - Adjust threshold values in `src/config/thresholds.h`

3. **Motor Tuning**
   - Test individual motors for direction
   - Calibrate speed and PWM values
   - Verify McNamum wheel coordination

---

## 📖 Usage Guide

### Operation Modes

#### 1. **Following Mode**
- Tracks and follows medical personnel
- Maintains 1.0-2.0m safe distance
- Uses color sensor for staff identification
- Automatically stops when staff stops

**Activation**: Select "Following Mode" via rotary encoder menu

#### 2. **Line Following Mode**
- Autonomous navigation along marked paths
- Junction detection for routing decisions
- ±2cm path deviation accuracy
- Emergency obstacle override

**Activation**: Select "Line Following" via menu

#### 3. **Monitoring Mode**
- Continuous patient vital signs tracking
- Environmental condition monitoring
- Real-time display on LCD
- Alert generation for abnormal readings

**Activation**: Automatic when patient sensor is connected

#### 4. **Emergency Mode**
- Immediate stop on button press
- Collision detection override
- Audio-visual alerts activated
- All movements cease

**Activation**: Press emergency button or automatic on obstacle collision

### Menu Navigation (KY-040 Rotary Encoder)

- **Rotate Clockwise/Counter-clockwise**: Navigate menu options
- **Press**: Confirm selection / Enter submenu
- **Long Press (>2 seconds)**: Return to main menu

### Adjustable Parameters

| Parameter | Range | Increment | Default |
|-----------|-------|-----------|---------|
| Following Distance | 0.5m - 2.0m | 0.1m | 1.5m |
| Robot Speed | 20% - 100% | 5% | 60% |
| Display Brightness | 0% - 100% | 5% | 80% |
| Buzzer Volume | 0 - 10 | 1 | 7 |
| Auto Mode | ON / OFF | Toggle | OFF |

### LCD Display Layout

```
[Mode] Battery: 87%
Following: 1.5m
HR: 72 SpO2: 98%
Status: Normal
```

**Line 1**: Current mode and battery percentage
**Line 2**: Mode-specific information (distance, speed, etc.)
**Line 3**: Patient vital signs (if monitoring)
**Line 4**: System status and alerts

---

## 📁 Project Structure

```
synapse/
├── ESP32-S3-Main/              # Main Controller Firmware
│   ├── main.cpp                # Main program entry point
│   ├── config/
│   │   ├── pins.h              # GPIO pin definitions
│   │   ├── constants.h         # System constants and thresholds
│   │   └── credentials.h       # WiFi and Firebase credentials
│   ├── sensors/
│   │   ├── mpu6050/            # 6-axis IMU sensor
│   │   ├── max30102/           # Pulse oximeter and heart rate
│   │   ├── tcs3200/            # RGB color recognition sensor
│   │   ├── lm393/              # IR line following sensor array
│   │   └── am2302/             # Temperature and humidity sensor
│   ├── actuators/
│   │   ├── lcd/                # ERMC1604SYG 16x4 LCD display
│   │   ├── buzzer/             # SFM27 piezo buzzer control
│   │   └── led/                # LE0066 LED array control
│   ├── ui/
│   │   ├── ky040/              # KY-040 rotary encoder interface
│   │   └── menu_system/        # Menu navigation and settings
│   ├── communication/
│   │   ├── uart/               # UART communication with WROOM32
│   │   ├── wifi_manager/       # WiFi connection management
│   │   ├── wifi_serial/        # Serial over WiFi debugging
│   │   └── firebase_manager/   # Firebase real-time sync
│   └── utils/
│       ├── battery/            # Battery monitoring and management
│       └── logger/             # System logging and debugging
│
├── ESP32-WROOM-Motor/          # Motor Controller Firmware
│   ├── main.cpp                # Motor control main program
│   ├── config/
│   │   ├── pins.h              # Motor driver GPIO pins
│   │   ├── constants.h         # Motor constants and limits
│   │   └── debug.h             # Debug configuration
│   ├── communication/
│   │   ├── uart.cpp            # UART receiver implementation
│   │   └── uart.h              # UART interface header
│   ├── motor/
│   │   ├── l298n.cpp/h         # L298N driver interface
│   │   ├── movement.cpp/h      # Movement patterns and control
│   │   └── speed.cpp/h         # Speed control and PWM
│   └── safety/
│       ├── emergency.cpp       # Emergency stop implementation
│       └── emergency.h         # Emergency stop interface
│
├── webapp/                     # Web Interface (GitHub Pages)
│   ├── css/
│   │   └── styles.css          # Custom styling
│   └── assets/
│       └── images/             # UI images and icons
│
├── hardware/                   # Hardware Designs
│   ├── circuit_diagrams/       # Wiring schematics
│   └── 3d_models/              # 3D printable parts
│
├── docs/                       # Documentation
│   ├── ESP 32 S3/
│   ├── ESP WROOM 32/
│   ├── flowcharts/             # System flowcharts
│   └── Synapse_Updated_2026/   # pin layout, bill of materials
│
├── .github/workflows/          # CI/CD pipelines
│   └── static.yml              # GitHub Pages deployment
│
├── README.md                   # This file
├── LICENSE                     # MIT License
├── index.html                  # Web app entry point
└── .gitignore                  # Git ignore rules
```

---

## 📊 Performance Metrics

### Current Performance Specifications

| Metric | Target | Achieved | Status |
|--------|--------|----------|--------|
| Operating Time | 8+ hours | 2+ hours | 🟡 In Progress |
| Transportation Workload Reduction | 60-70% | 65% | ✅ Achieved |
| Operational Uptime | 99% | 99% | ✅ Achieved |
| Collision Avoidance | 100% | 100% | ✅ Achieved |
| Maximum Speed | 0.5 m/s | 0.5 m/s | ✅ Achieved |
| Detection Range | 2-100 cm | 2-100 cm | ✅ Achieved |
| Navigation Precision | ±5 cm | ±2 cm | ✅ Exceeded |
| Emergency Stop Response | <1 second | <1 second | ✅ Achieved |
| Sensor Accuracy (Vital Signs) | ±2% | ±2% | ✅ Achieved |
| Following Distance Accuracy | ±10 cm | ±10 cm | ✅ Achieved |

### Sensor Specifications

**Ultrasonic Sensors (HC-SR04)**
- Range: 2-100 cm
- Response Time: <1 second
- Coverage: 360° (4 sensors)

**MPU6050 IMU**
- Gyroscope: ±250/500/1000/2000 °/s
- Accelerometer: ±2/4/8/16 g
- Update Rate: 100 Hz

**MAX30102 Pulse Oximeter**
- Heart Rate Range: 30-200 BPM
- SpO2 Range: 70-100%
- Accuracy: ±2%
- Update Interval: 1 second

**AM2302 Temperature/Humidity**
- Temperature Range: -40°C to +80°C
- Temperature Accuracy: ±0.5°C
- Humidity Range: 0-100% RH
- Humidity Accuracy: ±3% RH

**TCS3200 Color Sensor**
- Color Detection: 16.7 million colors
- Light-to-Frequency Conversion
- Programmable scaling

---

## 🌐 Web Interface

SYNAPSE includes a comprehensive web-based control and monitoring interface hosted on GitHub Pages.

### Access Information
- **Live Demo**: [https://heyisula.github.io/synapse/](https://heyisula.github.io/synapse/)
- **Demo Credentials**:
  - Email: `githubtesting@synapse.com`
  - Password: `testing1234`

### Features

#### Real-Time Dashboard
- Live sensor data visualization
- Patient vital signs monitoring
- Environmental conditions display
- Robot location and status
- Battery level indicator

#### Remote Control
- Movement commands (forward, backward, turn, stop)
- Speed adjustment
- Mode selection
- Emergency stop activation

#### Data Analytics
- Historical data visualization
- Usage pattern analysis
- Performance metrics
- Alert history

#### Settings Management
- WiFi configuration
- Sensor calibration parameters
- Alert thresholds
- User preferences

#### Authentication
- Secure login system
- Role-based access control
- Activity logging
- Session management

### Technology Stack
- **Frontend**: HTML5, CSS3, JavaScript (ES6+)
- **Backend**: Firebase Realtime Database
- **Hosting**: GitHub Pages
- **Analytics**: Google Analytics
- **Charts**: Chart.js / Recharts

---

## 🔧 Configuration

### Pin Assignments

#### ESP32-S3 Main Controller

| GPIO | Function | Description |
|------|----------|-------------|
| GPIO 1 | IR S5 | Line sensor (far right) |
| GPIO 2 | I2C SDA | LCD, MPU-6050, MAX30102 |
| GPIO 3 | I2C SCL | LCD, MPU-6050, MAX30102 |
| GPIO 4 | IR S1 | Line sensor (far left) |
| GPIO 5 | IR S2 | Line sensor (left) |
| GPIO 6 | IR S3 | Line sensor (center) |
| GPIO 7 | IR S4 | Line sensor (right) |
| GPIO 8 | US Front Trigger | Ultrasonic sensor (front) |
| GPIO 9 | US Front Echo | Ultrasonic sensor (front) |
| GPIO 10 | LDR 4 | Compartment security (ADC) |
| GPIO 11 | Battery Monitor | Battery voltage ADC |
| GPIO 12 | US Left Trigger | Ultrasonic sensor (left) |
| GPIO 13 | US Left Echo | Ultrasonic sensor (left) |
| GPIO 14 | US Right Echo | Ultrasonic sensor (right) |
| GPIO 15 | Buzzer 1 | Piezo buzzer output |
| GPIO 16 | Buzzer 2 | Piezo buzzer output |
| GPIO 17 | Color OUT | TCS3200 frequency output |
| GPIO 18 | Color S0 | TCS3200 frequency scaling |
| GPIO 19 | AM2302 Data | Temperature/humidity sensor |
| GPIO 20 | Color S3 | TCS3200 filter selection |
| GPIO 21 | US Right Trigger | Ultrasonic sensor (right) |
| GPIO 35 | LDR 1 | Path visibility left (ADC) |
| GPIO 36 | LDR 2 | Path visibility right (ADC) |
| GPIO 37 | LDR 3 | Compartment security (ADC) |
| GPIO 38 | Encoder CLK | KY-040 rotary encoder clock |
| GPIO 39 | Encoder DT | KY-040 rotary encoder data |
| GPIO 40 | Encoder SW | KY-040 push button |
| GPIO 41 | LED Left | LED array control |
| GPIO 42 | LED Right | LED array control |
| GPIO 43 | UART TX | Communication to WROOM32 |
| GPIO 44 | UART RX | Communication from WROOM32 |
| GPIO 45 | Color S1 | TCS3200 frequency scaling |
| GPIO 46 | Color S2 | TCS3200 filter selection |
| GPIO 47 | US Back Echo | Ultrasonic sensor (back) |
| GPIO 48 | US Back Trigger | Ultrasonic sensor (back) |

#### ESP32 WROOM-32 Motor Controller

| GPIO | Function | Description |
|------|----------|-------------|
| GPIO 0 | UART RX | Communication from S3 |
| GPIO 3 | UART TX | Communication to S3 |
| GPIO 4 | Motor LB Enable | Left back motor PWM |
| GPIO 5 | Motor RB Enable | Right back motor PWM |
| GPIO 12 | Motor LB IN1 | Left back motor direction |
| GPIO 13 | Motor LB IN2 | Left back motor direction |
| GPIO 14 | Motor RB IN1 | Right back motor direction |
| GPIO 15 | Motor RB IN2 | Right back motor direction |
| GPIO 16 | Motor LF IN1 | Left front motor direction |
| GPIO 17 | Motor LF IN2 | Left front motor direction |
| GPIO 18 | Motor RF IN1 | Right front motor direction |
| GPIO 19 | Motor RF IN2 | Right front motor direction |
| GPIO 22 | Motor LF Enable | Left front motor PWM |
| GPIO 23 | Motor RF Enable | Right front motor PWM |
| GPIO 27 | Emergency Button | Hardware interrupt input |

### I2C Device Addresses

| Device | Address | Purpose |
|--------|---------|---------|
| LCD Display (PCF8574) | 0x27 | 16×4 character display |
| MPU-6050 IMU | 0x68 | Gyroscope/accelerometer |
| MAX30102 Pulse Oximeter | 0x57 | Heart rate and SpO2 |

### WiFi Configuration

Edit `ESP32-S3-Main/src/config/wifi_config.h`:

```cpp
// WiFi credentials
#define WIFI_SSID "YourNetworkSSID"
#define WIFI_PASSWORD "YourNetworkPassword"

// Firebase configuration
#define FIREBASE_HOST "your-project.firebaseio.com"
#define FIREBASE_AUTH "your-firebase-auth-token"
```

---

## 🛡️ Safety Features

### Hardware Safety
- **Emergency Stop Button**: Hardware interrupt on GPIO 27 (WROOM32)
- **Immediate Motor Shutdown**: Direct connection to motor controller
- **Fail-Safe Design**: System defaults to safe state on power loss

### Collision Avoidance
- **360° Obstacle Detection**: 4× ultrasonic sensors
- **Automatic Braking**: <1 second response time
- **Audio-Visual Warnings**: Buzzers and LEDs alert nearby personnel
- **Safe Distance Maintenance**: 50cm minimum clearance

### System Monitoring
- **Battery Voltage Monitoring**: Low battery warnings and auto-shutdown
- **Sensor Health Checks**: Continuous validation of sensor readings
- **Watchdog Timers**: Automatic recovery from system hangs
- **Error Logging**: All errors recorded for debugging

### Patient Safety
- **Medical-Grade Accuracy**: ±2% accuracy for vital signs
- **Alert System**: Immediate notifications for abnormal readings
- **Redundant Monitoring**: Multiple sensors for critical functions
- **Fail-Safe Defaults**: System prioritizes patient safety

---

## 🤝 Contributing

Contributions are welcome! Here's how you can help:

### Reporting Issues
1. Check existing issues to avoid duplicates
2. Use issue templates when available
3. Include detailed reproduction steps
4. Attach relevant logs and screenshots

### Submitting Pull Requests

1. **Fork the repository**
```bash
git clone https://github.com/your-username/synapse.git
cd synapse
```

2. **Create a feature branch**
```bash
git checkout -b feature/amazing-feature
```

3. **Make your changes**
   - Follow existing code style
   - Add comments for complex logic
   - Update documentation as needed

4. **Test thoroughly**
   - Test on actual hardware when possible
   - Verify all existing functionality still works
   - Add unit tests for new features

5. **Commit your changes**
```bash
git commit -m "Add amazing feature"
```

6. **Push to your fork**
```bash
git push origin feature/amazing-feature
```

7. **Open a Pull Request**
   - Describe your changes in detail
   - Reference any related issues
   - Include screenshots/videos if relevant

### Development Guidelines

#### Code Style
- Use meaningful variable and function names
- Follow ESP32 Arduino naming conventions
- Keep functions focused and modular
- Add comments for complex algorithms

#### Documentation
- Update README.md for significant changes
- Document new features in `/docs`
- Include circuit diagrams for hardware changes
- Update pin assignments if modified

#### Testing
- Test each sensor/component independently
- Verify inter-controller communication
- Check all operation modes
- Test emergency stop functionality
- Validate web interface changes

---

## 🔮 Future Enhancements

### Short-Term Improvements
- [ ] Enhanced line following using computer vision
- [ ] Expanded battery capacity for 8+ hour operation
- [ ] Additional safety features (audible warnings, LED indicators)
- [ ] Mobile app for iOS and Android
- [ ] Voice command interface

### Long-Term Vision
- [ ] Multi-robot coordination for hospital-wide coverage
- [ ] AI-powere
