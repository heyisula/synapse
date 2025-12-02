#ifndef COLLISION_DETECTION_H
#define COLLISION_DETECTION_H

#include <Arduino.h>

enum CollisionZone {
    ZONE_FRONT,
    ZONE_BACK,
    ZONE_LEFT,
    ZONE_RIGHT,
    ZONE_NONE
};

enum CollisionSeverity {
    SEVERITY_NONE,
    SEVERITY_WARNING,
    SEVERITY_CRITICAL,
    SEVERITY_IMMINENT
};

struct CollisionStatus {
    bool detected;
    CollisionZone zone;
    CollisionSeverity severity;
    float distance;
    unsigned long detectionTime;
};

class CollisionDetector {
private:
    CollisionStatus currentStatus;
    
    // Detection thresholds (mm)
    float warningDistance;
    float criticalDistance;
    float imminentDistance;
    
    // History for trend detection
    float lastDistances[4];  // Front, Back, Left, Right
    unsigned long lastUpdateTime;
    
    // Collision prediction
    bool predictionEnabled;
    float approachVelocity;
    
public:
    CollisionDetector();
    void begin();
    
    void update(float frontDist, float backDist, float leftDist, float rightDist);
    
    CollisionStatus getStatus() { return currentStatus; }
    bool isCollisionDetected() { return currentStatus.detected; }
    CollisionSeverity getSeverity() { return currentStatus.severity; }
    
    void setThresholds(float warning, float critical, float imminent);
    void enablePrediction(bool enable) { predictionEnabled = enable; }
    
private:
    CollisionSeverity evaluateDistance(float distance);
    CollisionZone findClosestZone(float front, float back, float left, float right);
    float calculateApproachVelocity(float currentDist, float lastDist, float dt);
};

#endif