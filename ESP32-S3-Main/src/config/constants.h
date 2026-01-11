#ifndef CONSTANTS_H
#define CONSTANTS_H

// System Constants
#define SERIAL_BAUD_RATE 115200
#define UART_BAUD_RATE 115200
#define I2C_FREQUENCY 100000

// Ultrasonic Constants
#define MAX_ULTRASONIC_DISTANCE 200  // cm
#define MIN_SAFE_DISTANCE 20         // cm
#define SOUND_SPEED 0.034            // cm/microsecond

// Battery Constants
#define BATTERY_LOW_VOLTAGE 10.5     // V
#define BATTERY_CRITICAL_VOLTAGE 10.0 // V

// Timing Constants
#define SENSOR_READ_INTERVAL 50      // ms
#define DISPLAY_UPDATE_INTERVAL 100  // ms
#define HEARTBEAT_INTERVAL 1000      // ms

// Motor Speed Constants
#define MOTOR_SPEED_MIN 30            // %
#define MOTOR_SPEED_MAX 100           // %
#define MOTOR_SPEED_DEFAULT 50        // %

// Health Monitoring Constants
#define HR_MIN 60                    // BPM
#define HR_MAX 100                   // BPM
#define SPO2_MIN 95                  // %
#define SPO2_MAX 100                 // %

#define ALPHA 0.96f  // Start here, tune between 0.96-0.98

#endif