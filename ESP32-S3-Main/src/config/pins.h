#ifndef PINS_H
#define PINS_H

// I2C Bus
#define I2C_SDA 2
#define I2C_SCL 3

// Ultrasonic Sensors
#define ULTRASONIC_FRONT_TRIG 8
#define ULTRASONIC_FRONT_ECHO 9
#define ULTRASONIC_BACK_TRIG 10
#define ULTRASONIC_BACK_ECHO 11
#define ULTRASONIC_LEFT_TRIG 12
#define ULTRASONIC_LEFT_ECHO 13
#define ULTRASONIC_RIGHT_TRIG 21
#define ULTRASONIC_RIGHT_ECHO 47

// 5-Bit Line Follower Sensor
#define LINE_SENSOR_S1 16
#define LINE_SENSOR_S2 4   
#define LINE_SENSOR_S3 5   
#define LINE_SENSOR_S4 6   
#define LINE_SENSOR_S5 7   

// LDR Sensors (Using ADC2 channels)
#define LDR_PATH_LEFT 15       
#define LDR_PATH_RIGHT 41      
#define LDR_COMPARTMENT_1 17   
#define LDR_COMPARTMENT_2 18   

// Battery Monitor (ADC)
#define BATTERY_ADC 46

// Buzzers
#define BUZZER_1 35
#define BUZZER_2 36

// UART Communication
#define UART_TX 43
#define UART_RX 44

// LED Arrays
#define LED_RIGHT 42
#define LED_LEFT 1

// Rotary Encoder
#define ROTARY_SW 40
#define ROTARY_DT 39
#define ROTARY_CLK 38

// Color Sensor (TCS3200/TCS230)
#define COLOR_OUT 14
#define COLOR_S0 48
#define COLOR_S1 45
#define COLOR_S2 20
#define COLOR_S3 37

// Temperature/Humidity Sensor (DHT22/AM2302)
#define AM2302_DATA 19

#endif