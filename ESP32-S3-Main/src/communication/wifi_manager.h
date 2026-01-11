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

    void checkConnection();

public:
    WiFiManager();
    void begin(const char* ssid, const char* password);
    void update();

    bool isConnected() { return connected; }
    WiFiStatus getStatus();

    void disconnect();
    void reconnect();
};

#endif
