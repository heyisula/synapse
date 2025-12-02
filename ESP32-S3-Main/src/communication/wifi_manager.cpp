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
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        connected = true;
        Serial.println("\nWiFi Connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        connected = false;
        Serial.println("\nWiFi Connection Failed!");
    }
}

void WiFiManager::update() {
    checkConnection();
}

void WiFiManager::checkConnection() {
    bool currentlyConnected = (WiFi.status() == WL_CONNECTED);
    
    if (currentlyConnected != connected) {
        connected = currentlyConnected;
        
        if (connected) {
            Serial.println("WiFi reconnected!");
        } else {
            Serial.println("WiFi disconnected!");
        }
    }
    
    // Auto-reconnect
    if (!connected && (millis() - lastReconnectAttempt > reconnectInterval)) {
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

void WiFiManager::sendData(String topic, String data) {
    if (!connected) {
        Serial.println("Cannot send data - WiFi not connected");
        return;
    }
    
    // In a real implementation, this would send to MQTT or HTTP endpoint
    Serial.print("WiFi TX [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.println(data);
}

void WiFiManager::sendAlert(String message, int priority) {
    if (!connected) {
        Serial.println("Cannot send alert - WiFi not connected");
        return;
    }
    
    String priorityStr;
    switch(priority) {
        case 1: priorityStr = "LOW"; break;
        case 2: priorityStr = "MEDIUM"; break;
        case 3: priorityStr = "HIGH"; break;
        case 4: priorityStr = "CRITICAL"; break;
        default: priorityStr = "INFO";
    }
    
    Serial.print("WiFi ALERT [");
    Serial.print(priorityStr);
    Serial.print("]: ");
    Serial.println(message);
    
    // Send to hospital monitoring system
    sendData("alerts", "{\"priority\":\"" + priorityStr + 
             "\",\"message\":\"" + message + "\"}");
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