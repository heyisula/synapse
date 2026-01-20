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

    // Non-blocking initialization - authentication happens asynchronously
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    initialized = true;
}
bool FirebaseManager::ready() {
    return initialized && Firebase.ready();
}

void FirebaseManager::update() {
    
}

bool FirebaseManager::sendData(const FirebaseTxData& d) {
    if (!ready()) return false;

    FirebaseJson json;

    // Sensors - SEND ONLY
    json.set("acceleration", d.acceleration);
    json.set("angular", d.angular);
    json.set("battery", d.battery);
    
    // DEBUG: Print voltage value before sending to Firebase
    Serial.print("📤 Firebase TX - Voltage: ");
    Serial.print(d.voltage, 4); // Print with 4 decimal places
    Serial.print(" (type: float, raw bytes: ");
    Serial.print(*(uint32_t*)&d.voltage, HEX);
    Serial.println(")");
    
    json.set("voltage", d.voltage);

    // Environment - SEND ONLY
    json.set("temp", d.temp);
    json.set("humidity", d.humidity);
    json.set("lightlevel", d.lightlevel);

    // Health - SEND ONLY
    json.set("hr", d.hr);
    json.set("sp02", d.sp02);

    // Ultrasonic - SEND ONLY
    json.set("ultrasonic_center", d.ultrasonic_center);
    json.set("ultrasonic_left", d.ultrasonic_left);
    json.set("ultrasonic_rear", d.ultrasonic_rear);
    json.set("ultrasonic_right", d.ultrasonic_right);

    // Color - SEND ONLY
    json.set("colour", d.colour);

    // Compartment - SEND ONLY
    json.set("compartment", d.compartment);

    if (!Firebase.updateNode(fbdo, "/", json)) {
        Serial.print("Firebase TX failed: ");
        Serial.println(fbdo.errorReason());
        return false;
    }

    return true;
}

bool FirebaseManager::receiveData(FirebaseRxData& d) {
    if (!ready()) return false;

    static unsigned long lastRX = 0;
    if (millis() - lastRX < 1000) return true; // Keep old data if less than 1s passed
    lastRX = millis();

    if (!Firebase.getJSON(fbdo, "/")) {
        Serial.print("Firebase RX failed: ");
        Serial.println(fbdo.errorReason());
        return false;
    }

    FirebaseJson& json = fbdo.jsonObject();
    FirebaseJsonData jd;

    // Buzzer control - RECEIVE ONLY
    json.get(jd, "buzzer01ring");
    if (jd.success) d.buzzer01ring = jd.boolValue;

    json.get(jd, "buzzer02ring");
    if (jd.success) d.buzzer02ring = jd.boolValue;

    json.get(jd, "buzzersound");
    if (jd.success) d.buzzersound = jd.intValue;


    // LED brightness control - RECEIVE ONLY
    json.get(jd, "lightadj_left");
    if (jd.success) d.lightadj_left = jd.intValue;

    json.get(jd, "lightadj_right");
    if (jd.success) d.lightadj_right = jd.intValue;


    // Sensor start/stop flags - RECEIVE ONLY
    json.get(jd, "colour_start");
    if (jd.success) d.colour_start = jd.boolValue;

    json.get(jd, "compartment_start");
    if (jd.success) d.compartment_start = jd.boolValue;

    json.get(jd, "heartrate_start");
    if (jd.success) d.heartrate_start = jd.boolValue;

    json.get(jd, "ultrasonic_start");
    if (jd.success) d.ultrasonic_start = jd.boolValue;

    return true;
}