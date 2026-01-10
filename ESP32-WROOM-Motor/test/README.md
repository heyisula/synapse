# Hardware Diagnostic Tests

This folder contains **Hardware Diagnostic Scripts** designed to verify your wiring, motors, and communication before running the main firmware. Unlike standard unit tests, these are **interactive sketches** that run indefinitely to help you debug physical issues.

## 🚀 How to Run a Test
You can run a specific test using the PlatformIO command line. 
Make sure your ESP32 is connected.

**Command Syntax:**
```bash
pio test --without-uploading --without-testing
# (Note: Standard pio test expects Unity. Since these are loop-based diagnostics, 
#  Run them by uploading as main firmware or using the test command with specific filters)
```

**Recommended Method (Easiest):**
Since these scripts use `setup()` and `loop()`, the easiest way to run them is to **temporarily copy the content** of the desired test file into `src/main.cpp` and click "Upload".

## 📂 Test Descriptions

### 1. `test1_hardware_pins.cpp`
*   **Purpose**: Checks if all GPIO pins are working and connected.
*   **Action**: Toggles every Motor Pin (IN1, IN2, ENA...) High/Low every 200ms.
*   **What to look for**: Use a multimeter or LED to verify voltage changes on the pins. Also checks if the Emergency Button is pressed.

### 2. `test2_individual_motors.cpp`
*   **Purpose**: Verifies motor wiring polarity and driver health.
*   **Action**: Spins each motor (Left-Front, Left-Back, etc.) one by one in Forward and Reverse.
*   **What to look for**: Check if wheels spin in the correct direction. If a wheel spins backwards when it says "Forward", swap the motor wires.

### 3. `test3_uart_loopback.cpp`
*   **Purpose**: Tests the Serial Communication path.
*   **Action**: echoes back any data sent to the RX pin.
*   **Setup**: You might need to bridge TX and RX pins or send data via a USB-Serial adapter to verify it receives/sends correctly.

### 4. `test4_emergency_stop.cpp`
*   **Purpose**: Tests the Safety Interrupt system.
*   **Action**: Runs motors continuously until you press the Emergency Button.
*   **What to look for**: Motors should STOP immediately when button is pressed and stay stopped.

### 5. `test5_integration_test.cpp`
*   **Purpose**: Full system simulation.
*   **Action**: Simulates receiving a "Forward" command, accelerating, turning, and stopping.
*   **What to look for**: The robot should perform a "dance" (Forward -> Turn -> Stop) repeatedly.

### 6. `test6_diagnostic_monitor.cpp`
*   **Purpose**: Real-time sensor dashboard.
*   **Action**: Prints the status of all Inputs (Buttons, Sensors) and internal states to the Serial Monitor every 100ms.
*   **Usage**: Run this and open Serial Monitor to watch values change as you press buttons or trigger sensors.

### 7. `test7_serial_control.cpp`
*   **Purpose**: Manual Control via Keyboard.
*   **Action**: Allows you to type WASD keys in the Serial Monitor to drive the robot.
*   **Usage**: Send 'w' (Forward), 's' (Backward), 'a' (Left), 'd' (Right), 'x' (Stop).

---
**⚠️ MEMORY NOTE:**
Only ONE of these can be active at a time. The ESP32 cannot run `src/main.cpp` and `test/test1...` simultaneously.
