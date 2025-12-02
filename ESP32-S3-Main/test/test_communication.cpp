#ifndef PINS_H
#define PINS_H

// I2C Bus
#define SDA_PIN 2
#define SCL_PIN 3

// Ultrasonic Sensors
#define ULTRASONIC_FRONT_TRIG 4
#define ULTRASONIC_FRONT_ECHO 5
#define ULTRASONIC_BACK_TRIG 6
#define ULTRASONIC_BACK_ECHO 7
#define ULTRASONIC_LEFT_TRIG 8
#define ULTRASONIC_LEFT_ECHO 9
#define ULTRASONIC_RIGHT_TRIG 10
#define ULTRASONIC_RIGHT_ECHO 11

// Line Following Sensor (5-bit)
#define LINE_SENSOR_S1 35  // Far Left
#define LINE_SENSOR_S2 36  // Left
#define LINE_SENSOR_S3 37  // Center
#define LINE_SENSOR_S4 1   // Right
#define LINE_SENSOR_S5 21  // Far Right

// Color Sensor (TCS3200)
#define COLOR_S0 47
#define COLOR_S1 48
#define COLOR_S2 20
#define COLOR_S3 45
#define COLOR_OUT 0

// Environmental Sensor
#define AM2320_PIN 19

// LDR Sensors
#define LDR_PATH_LEFT 15
#define LDR_PATH_RIGHT 16
#define LDR_STORAGE_1 17
#define LDR_STORAGE_2 18

// Rotary Encoder (KY-040)
#define ENCODER_CLK 38
#define ENCODER_DT 39
#define ENCODER_SW 40

// LED Arrays
#define LED_ARRAY_LEFT 41
#define LED_ARRAY_RIGHT 42

// Buzzers
#define BUZZER_1 12
#define BUZZER_2 13

// UART to ESP32-WROOM
#define UART_TX_PIN 43
#define UART_RX_PIN 44

#endif