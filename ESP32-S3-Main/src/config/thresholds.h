#ifndef THRESHOLDS_H
#define THRESHOLDS_H

// Environmental Thresholds
#define TEMP_MIN 20.0                // °C
#define TEMP_MAX 25.0                // °C
#define HUMIDITY_MIN 40.0            // %
#define HUMIDITY_MAX 60.0            // %
#define LIGHT_MIN 200                // Lux
#define LIGHT_MAX 800                // Lux

// Color Sensor Thresholds
#define BLACK_SUM_MAX 30
#define WHITE_R_MIN 25
#define WHITE_G_MIN 25
#define WHITE_B_MIN 30
#define WHITE_BALANCE_TOLERANCE 15
#define BLUE_B_MIN 18
#define BLUE_DIFF_MIN 5
#define GREEN_G_MIN 12
#define GREEN_DIFF_MIN 3
#define RED_R_MIN 20
#define RED_DIFF_MIN 10
// Temporal averaging
#define COLOR_AVG_SAMPLES 5
#define AMBIENT_ADAPT_RATE 0.05f
#define AMBIENT_UPDATE_THRESHOLD 15


// Collision Detection
#define COLLISION_DISTANCE_FRONT 50  // cm
#define COLLISION_DISTANCE_BACK 20   // cm
#define COLLISION_DISTANCE_SIDE 50   // cm
#define EMERGENCY_STOP_DISTANCE 50   // cm

// Compartment Light Threshold
#define COMPARTMENT_LIGHT_THRESHOLD 1000  // ADC value

//Path Visibility Threshold
#define PATH_LIGHT_THRESHOLD 3000        // ADC value

#define AM2303_READ_INTERVAL 2000UL

#define STATIONARY_GYRO_THRESHOLD 1.0f   // deg/s, ignore tiny gyro noise
#define STATIONARY_TILT_THRESHOLD 2.0f   // degrees, small tilt allowed
#define STATIONARY_COUNT_THRESHOLD 5     // number of consecutive readings before moving=NO

#endif