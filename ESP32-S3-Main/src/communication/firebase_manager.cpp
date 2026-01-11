#include "firebase_manager.h"

FirebaseManager::FirebaseManager() : initialized(false) {}

void FirebaseManager::begin(const char* apiKey,
                            const char* databaseUrl,
                            const char* userEmail,
                            const char* userPassword) {

    config.api_key = apiKey;
    config.database_url = databaseUrl;

    auth.user.email = userEmail;
    auth.user.password = userPassword;

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    Serial.println("Firebase Connected!");
    initialized = true;
}

void FirebaseManager::update() {
    // Reserved for future background tasks if needed
}

bool FirebaseManager::ready() {
    return initialized && Firebase.ready();
}

void FirebaseManager::sendData(const FirebaseTxData& d) {
    if (!ready()) return;

    Firebase.setInt (fbdo, "/sensors/acceleration", d.acceleration);
    Firebase.setInt (fbdo, "/sensors/angular", d.angular);
    Firebase.setInt (fbdo, "/sensors/battery", d.battery);
    Firebase.setInt (fbdo, "/sensors/voltage", d.voltage);

    Firebase.setInt (fbdo, "/environment/temp", d.temp);
    Firebase.setInt (fbdo, "/environment/humidity", d.humidity);
    Firebase.setInt (fbdo, "/environment/lightlevel", d.lightlevel);

    Firebase.setInt (fbdo, "/light/left", d.lightadj_left);
    Firebase.setInt (fbdo, "/light/right", d.lightadj_right);

    Firebase.setInt (fbdo, "/health/hr", d.hr);
    Firebase.setInt (fbdo, "/health/sp02", d.sp02);
    Firebase.setBool(fbdo, "/health/start", d.heartrate_start);

    Firebase.setInt (fbdo, "/ultrasonic/center", d.ultrasonic_center);
    Firebase.setInt (fbdo, "/ultrasonic/left", d.ultrasonic_left);
    Firebase.setInt (fbdo, "/ultrasonic/rear", d.ultrasonic_rear);
    Firebase.setInt (fbdo, "/ultrasonic/right", d.ultrasonic_right);
    Firebase.setBool(fbdo, "/ultrasonic/start", d.ultrasonic_start);

    Firebase.setBool(fbdo, "/buzzer/one", d.buzzer01ring);
    Firebase.setBool(fbdo, "/buzzer/two", d.buzzer02ring);
    Firebase.setInt (fbdo, "/buzzer/sound", d.buzzersound);

    Firebase.setString(fbdo, "/led/colour", d.colour);
    Firebase.setBool  (fbdo, "/led/start", d.colour_start);

    Firebase.setInt (fbdo, "/compartment/id", d.compartment);
    Firebase.setBool(fbdo, "/compartment/start", d.compartment_start);
}

bool FirebaseManager::receiveData(FirebaseRxData& d) {
    if (!ready()) return false;

    if (Firebase.getBool(fbdo, "/buzzer/one"))
        d.buzzer01ring = fbdo.boolData();

    if (Firebase.getBool(fbdo, "/buzzer/two"))
        d.buzzer02ring = fbdo.boolData();

    if (Firebase.getInt(fbdo, "/buzzer/sound"))
        d.buzzersound = fbdo.intData();

    if (Firebase.getString(fbdo, "/led/colour"))
        d.colour = fbdo.stringData();

    if (Firebase.getBool(fbdo, "/led/start"))
        d.colour_start = fbdo.boolData();

    if (Firebase.getInt(fbdo, "/compartment/id"))
        d.compartment = fbdo.intData();

    if (Firebase.getBool(fbdo, "/compartment/start"))
        d.compartment_start = fbdo.boolData();

    if (Firebase.getBool(fbdo, "/health/start"))
        d.heartrate_start = fbdo.boolData();

    if (Firebase.getBool(fbdo, "/ultrasonic/start"))
        d.ultrasonic_start = fbdo.boolData();

    return true;
}