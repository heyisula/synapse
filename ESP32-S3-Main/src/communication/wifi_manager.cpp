#include "wifi_manager.h"

WiFiManager::WiFiManager() 
    : connected(false), ssid(""), password(""),
      lastReconnectAttempt(0), reconnectInterval(30000) {}

void WiFiManager::begin(const char* wifiSSID, const char* wifiPassword) {
    ssid = String(wifiSSID);
    password = String(wifiPassword);

    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    lastReconnectAttempt = millis();  // track time for non-blocking reconnect
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
        } else {
            Serial.println("WiFi disconnected!");
        }
    }

    // Non-blocking reconnect: only attempt after interval
    if (!connected && (millis() - lastReconnectAttempt >= reconnectInterval)) {
        lastReconnectAttempt = millis();
        reconnect();
    }
}

WiFiStatus WiFiManager::getStatus() {
    WiFiStatus status;
    status.connected = connected;
    status.ssid = ssid;
    status.rssi = WiFi.RSSI();
    status.ipAddress = WiFi.localIP().toString();
    return status;
}

void WiFiManager::disconnect() {
    WiFi.disconnect();
    connected = false;
    Serial.println("WiFi disconnected");
}

void WiFiManager::reconnect() {
    Serial.println("Attempting WiFi reconnection...");
    WiFi.reconnect();
}
