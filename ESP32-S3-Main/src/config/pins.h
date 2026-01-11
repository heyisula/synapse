#ifndef PINS_H
#define PINS_H

// I2C Bus
#define I2C_SDA 2
#define I2C_SCL 3

// Ultrasonic Sensors
#define ULTRASONIC_FRONT_TRIG 8
#define ULTRASONIC_FRONT_ECHO 9
#define ULTRASONIC_BACK_TRIG 48
#define ULTRASONIC_BACK_ECHO 47
#define ULTRASONIC_LEFT_TRIG 12
#define ULTRASONIC_LEFT_ECHO 13
#define ULTRASONIC_RIGHT_TRIG 21
#define ULTRASONIC_RIGHT_ECHO 14

// 5-Bit Line Follower Sensor
#define LINE_SENSOR_S1 4
#define LINE_SENSOR_S2 5 
#define LINE_SENSOR_S3 6  
#define LINE_SENSOR_S4 7
#define LINE_SENSOR_S5 1   

// LDR Sensors (Using ADC2 channels)
#define LDR_PATH_LEFT 35       
#define LDR_PATH_RIGHT 36      
#define LDR_COMPARTMENT_1 37   
#define LDR_COMPARTMENT_2 10

// Battery Monitor (ADC)
#define BATTERY_ADC 11

// Buzzers
#define BUZZER_1 15
#define BUZZER_2 16

// UART Communication
#define UART_TX 43
#define UART_RX 44

// LED Arrays
#define LED_RIGHT 42
#define LED_LEFT 41

// Rotary Encoder
#define ROTARY_SW 40
#define ROTARY_DT 39
#define ROTARY_CLK 38

// Color Sensor (TCS3200/TCS230)
#define COLOR_OUT 17
#define COLOR_S0 18
#define COLOR_S1 45
#define COLOR_S2 46
#define COLOR_S3 20

// Temperature/Humidity Sensor (DHT22/AM2302)
#define AM2302_DATA 19

#endif