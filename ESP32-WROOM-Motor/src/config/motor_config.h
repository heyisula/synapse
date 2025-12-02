#ifndef MOTOR_CONFIG_H
#define MOTOR_CONFIG_H

// Motor configuration parameters
#define MOTOR_PWM_FREQUENCY 1000  // 1kHz
#define MOTOR_PWM_RESOLUTION 8    // 8-bit (0-255)

// Speed limits
#define MOTOR_MAX_SPEED 255
#define MOTOR_MIN_SPEED 30       // Minimum speed for motors to move

// Acceleration limits (PWM units per 10ms)
#define MOTOR_MAX_ACCEL 10
#define MOTOR_MAX_DECEL 20

// Stall detection
#define MOTOR_STALL_TIMEOUT 2000  // ms
#define MOTOR_STALL_SPEED_THRESHOLD 50

// Safety
#define MOTOR_WATCHDOG_TIMEOUT 1000  // ms
#define MOTOR_EMERGENCY_STOP_ACTIVE LOW

#endif