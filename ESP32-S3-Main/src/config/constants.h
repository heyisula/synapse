#ifndef CONSTANTS_H
#define CONSTANTS_H

// System Constants
#define SERIAL_BAUD_RATE 115200
#define UART_BAUD_RATE 115200
#define I2C_FREQUENCY 100000

// Ultrasonic Constants
#define MAX_ULTRASONIC_DISTANCE 100  // cm
#define MIN_SAFE_DISTANCE 20         // cm
#define SOUND_SPEED 0.034            // cm/microsecond

// Battery Constants
#define BATTERY_LOW_VOLTAGE 12.5     // V (Approx 3.12V/cell - entering critical zone)
#define BATTERY_CRITICAL_VOLTAGE 12.0 // V (Approx 3.0V/cell - empty)

// Timing Constants
#define SENSOR_READ_INTERVAL 50      // ms
#define DISPLAY_UPDATE_INTERVAL 100  // ms
#define HEARTBEAT_INTERVAL 1000      // ms

// Motor Speed Constants
#define MOTOR_SPEED_MIN 40            // %
#define MOTOR_SPEED_MAX 100           // %
#define MOTOR_SPEED_DEFAULT 50        // %

// Health Monitoring Constants
#define HR_MIN 60                    // BPM
#define HR_MAX 100                   // BPM
#define SPO2_MIN 95                  // %
#define SPO2_MAX 100                 // %

#define MPU_ALPHA 0.96f  // Complementary filter constant

#endif