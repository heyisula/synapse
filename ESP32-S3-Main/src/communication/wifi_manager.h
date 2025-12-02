#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

struct WiFiStatus {
    bool connected;
    String ssid;
    int rssi;
    String ipAddress;
};

class WiFiManager {
private:
    bool connected;
    String ssid;
    String password;
    unsigned long lastReconnectAttempt;
    int reconnectInterval;
    
public:
    WiFiManager();
    void begin(const char* ssid, const char* password);
    void update();
    
    bool isConnected() { return connected; }
    WiFiStatus getStatus();
    
    void sendData(String topic, String data);
    void sendAlert(String message, int priority);
    
    void disconnect();
    void reconnect();
    
private:
    void checkConnection();
};

#endif