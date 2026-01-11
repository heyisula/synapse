
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

// Following Mode Constants
#define FOLLOW_DISTANCE_MIN 50       // cm
#define FOLLOW_DISTANCE_MAX 200      // cm
#define FOLLOW_DISTANCE_DEFAULT 100  // cm

// Navigation Constants
#define LINE_THRESHOLD 512           // Analog threshold for line detection
#define MAX_WAYPOINTS 10
#define POSITION_PRECISION 5         // cm

// Battery Constants
#define BATTERY_LOW_VOLTAGE 10.5     // V
#define BATTERY_CRITICAL_VOLTAGE 10.0 // V

// Timing Constants
#define SENSOR_READ_INTERVAL 50      // ms
#define DISPLAY_UPDATE_INTERVAL 100  // ms
#define HEARTBEAT_INTERVAL 1000      // ms

// Motor Speed Constants
#define MOTOR_SPEED_MIN 20           // %
#define MOTOR_SPEED_MAX 100          // %
#define MOTOR_SPEED_DEFAULT 50       // %

// Health Monitoring Constants
#define HR_MIN 60                    // BPM
#define HR_MAX 100                   // BPM
#define SPO2_MIN 95                  // %
#define SPO2_MAX 100                 // %

//Buzzer Constants
#define CONFIRM_FREQ   500
#define WARNING_FREQ   4000
#define ERROR_FREQ     8000
#define EMERGENCY_FREQ 2600

#endif