#ifndef THRESHOLDS_H
#define THRESHOLDS_H

// Environmental Thresholds
#define TEMP_MIN 20.0                // °C
#define TEMP_MAX 25.0                // °C
#define HUMIDITY_MIN 40.0            // %
#define HUMIDITY_MAX 60.0            // %
#define LIGHT_MIN 200                // Lux
#define LIGHT_MAX 800                // Lux

// Color Detection Thresholds
#define WHITE_R_MIN 200
#define WHITE_G_MIN 200
#define WHITE_B_MIN 200
#define BLUE_B_MIN 150
#define BLUE_R_MAX 100
#define RED_R_MIN 150
#define RED_G_MAX 100
#define BLACK_SUM_MAX 150

// Collision Detection
#define COLLISION_DISTANCE_FRONT 30  // cm
#define COLLISION_DISTANCE_SIDE 20   // cm
#define EMERGENCY_STOP_DISTANCE 15   // cm

#endif