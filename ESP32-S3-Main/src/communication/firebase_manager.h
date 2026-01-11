#ifndef FIREBASE_MANAGER_H
#define FIREBASE_MANAGER_H

#include <Arduino.h>
#include <FirebaseESP32.h>

struct FirebaseTxData {
    int acceleration;
    int angular;
    int battery;

    bool buzzer01ring;
    bool buzzer02ring;
    int  buzzersound;

    String colour;
    bool   colour_start;

    int  compartment;
    bool compartment_start;

    bool heartrate_start;
    int  hr;

    int humidity;

    int lightadj_left;
    int lightadj_right;
    int lightlevel;

    int sp02;
    int temp;

    int ultrasonic_center;
    int ultrasonic_left;
    int ultrasonic_rear;
    int ultrasonic_right;
    bool ultrasonic_start;

    int voltage;
};

struct FirebaseRxData {
    bool buzzer01ring;
    bool buzzer02ring;
    int  buzzersound;

    String colour;
    bool colour_start;

    int compartment;
    bool compartment_start;

    bool heartrate_start;
    bool ultrasonic_start;
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

    void sendData(const FirebaseTxData& data);
    bool receiveData(FirebaseRxData& data);
};

#endif
