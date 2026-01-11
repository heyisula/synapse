#include "wifi_manager.h"

WiFiManager::WiFiManager() : 
        connected(false),
        ssid(""), 
        password(""),
        lastReconnectAttempt(0), reconnectInterval(10000), reconnectAttempts(0) {}

void WiFiManager::begin(const char* wifiSSID, const char* wifiPassword) {
    ssid = String(wifiSSID);
    password = String(wifiPassword);

    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    lastReconnectAttempt = millis();
}

void WiFiManager::update() {
    checkConnection();
}

void WiFiManager::checkConnection() {
    bool currentlyConnected = (WiFi.status() == WL_CONNECTED);

    if (currentlyConnected != connected) {
        connected = currentlyConnected;

        if (connected) {
            Serial.println("WiFi connected!");
            Serial.print("IP Address: ");
            Serial.println(WiFi.localIP());
            reconnectAttempts = 0;
        } else {
            Serial.println("WiFi disconnected!");
        }
    }

    if (!connected && (millis() - lastReconnectAttempt >= reconnectInterval)) {
        lastReconnectAttempt = millis();
        reconnect();
    }
}

WiFiStatus WiFiManager::getStatus() const {
    WiFiStatus status;
    status.connected = connected;
    status.ssid = ssid;
    status.rssi = WiFi.RSSI();
    status.ipAddress = connected ? WiFi.localIP().toString() : "0.0.0.0";
    return status;
}

void WiFiManager::disconnect() {
    WiFi.disconnect(true);
    connected = false;
    reconnectAttempts = 0;
    Serial.println("WiFi disconnected manually");
}

void WiFiManager::reconnect() {
    reconnectAttempts++;
    Serial.print("Attempting WiFi reconnection, attempt #");
    Serial.println(reconnectAttempts);
    WiFi.begin(ssid.c_str(), password.c_str());
}
