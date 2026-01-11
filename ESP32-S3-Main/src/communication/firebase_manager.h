#ifndef FIREBASE_MANAGER_H
#define FIREBASE_MANAGER_H

#include <Arduino.h>
#include <FirebaseESP32.h>

struct FirebaseTxData {
    // Sensor readings - SEND ONLY
    int acceleration;           // Send acceleration
    int angular;               // Send angular velocity
    int battery;               // Send battery level
    int voltage;               // Send voltage
    
    // Environment - SEND ONLY
    int temp;                  // Send temperature
    int humidity;              // Send humidity
    int lightlevel;            // Send light amount from path LDRs
    
    // Health - SEND ONLY
    int hr;                    // Send heart rate
    int sp02;                  // Send SpO2 level
    
    // Ultrasonic - SEND ONLY
    int ultrasonic_center;     // Send center distance
    int ultrasonic_left;       // Send left distance
    int ultrasonic_rear;       // Send rear distance
    int ultrasonic_right;      // Send right distance
    
    // Color - SEND ONLY
    String colour;             // Send detected color (RED/BLUE/GREEN/WHITE/UNKNOWN)
    
    // Compartment - SEND ONLY
    int compartment;           // Send compartment state (0=open, 255=closed)
};

struct FirebaseRxData {
    // Buzzer control - RECEIVE ONLY
    bool buzzer01ring;         // Receive signal to ring buzzer 1
    bool buzzer02ring;         // Receive signal to ring buzzer 2
    int buzzersound;           // Receive buzzer volume control
    
    // LED brightness control - RECEIVE ONLY
    int lightadj_left;         // Receive left LED brightness adjustment
    int lightadj_right;        // Receive right LED brightness adjustment
    
    // Sensor start/stop flags - RECEIVE ONLY
    bool colour_start;         // Receive signal to start color detection
    bool compartment_start;    // Receive signal to start compartment LDR check
    bool heartrate_start;      // Receive signal to start heart rate sensor
    bool ultrasonic_start;     // Receive signal to start ultrasonic sensors
};

class FirebaseManager {
private:
    FirebaseData fbdo;
    FirebaseAuth auth;
    FirebaseConfig config;

    bool initialized;

public:
    FirebaseManager();

    void begin(const char* apiKey,
               const char* databaseUrl,
               const char* userEmail,
               const char* userPassword);

    void update();
    bool ready();

    bool sendData(const FirebaseTxData& data);
    bool receiveData(FirebaseRxData& data);
};

#endif
