#ifndef PINS_H
#define PINS_H

//I2C Bus
#define I2C_SDA 2
#define I2C_SCL 3

// Ultrasonic Sensors
#define ULTRASONIC_FRONT_TRIG 4
#define ULTRASONIC_FRONT_ECHO 5
#define ULTRASONIC_BACK_TRIG 6
#define ULTRASONIC_BACK_ECHO 7
#define ULTRASONIC_LEFT_TRIG 8
#define ULTRASONIC_LEFT_ECHO 9
#define ULTRASONIC_RIGHT_TRIG 10
#define ULTRASONIC_RIGHT_ECHO 11

// LDR Sensors
#define LDR_PATH_LEFT 15
#define LDR_PATH_RIGHT 16
#define LDR_COMPARTMENT_1 17
#define LDR_COMPARTMENT_2 18

// Buzzers
#define BUZZER_1 12
#define BUZZER_2 13

// UART Communication
#define UART_TX 43
#define UART_RX 44

// 5-Bit Line Follower Sensor
#define LINE_SENSOR_S1 35  // Far Left
#define LINE_SENSOR_S2 36  // Left
#define LINE_SENSOR_S3 37  // Center
#define LINE_SENSOR_S4 1   // Right
#define LINE_SENSOR_S5 21  // Far Right

// LED Arrays
#define LED_RIGHT 42
#define LED_LEFT 41

// Rotary Encoder
#define ROTARY_SW 40
#define ROTARY_DT 39
#define ROTARY_CLK 38

// Color Sensor
#define COLOR_OUT 0
#define COLOR_S0 47
#define COLOR_S1 48
#define COLOR_S2 20
#define COLOR_S3 45

// Temperature/Humidity Sensor
#define AM2302_DATA 19

#endif