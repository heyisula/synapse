#include "collision_detection.h"

CollisionDetector::CollisionDetector() 
    : warningDistance(500), criticalDistance(300), imminentDistance(150),
      lastUpdateTime(0), predictionEnabled(true), approachVelocity(0) {
    currentStatus.detected = false;
    currentStatus.zone = ZONE_NONE;
    currentStatus.severity = SEVERITY_NONE;
    currentStatus.distance = 9999;
    
    for (int i = 0; i < 4; i++) {
        lastDistances[i] = 9999;
    }
}

void CollisionDetector::begin() {
    lastUpdateTime = millis();
}

void CollisionDetector::update(float frontDist, float backDist, 
                               float leftDist, float rightDist) {
    unsigned long now = millis();
    float dt = (now - lastUpdateTime) / 1000.0;
    lastUpdateTime = now;
    
    // Find closest obstacle
    currentStatus.zone = findClosestZone(frontDist, backDist, leftDist, rightDist);
    
    float closestDistance = 9999;
    switch (currentStatus.zone) {
        case ZONE_FRONT: closestDistance = frontDist; break;
        case ZONE_BACK: closestDistance = backDist; break;
        case ZONE_LEFT: closestDistance = leftDist; break;
        case ZONE_RIGHT: closestDistance = rightDist; break;
        default: closestDistance = 9999;
    }
    
    currentStatus.distance = closestDistance;
    currentStatus.severity = evaluateDistance(closestDistance);
    currentStatus.detected = (currentStatus.severity > SEVERITY_NONE);
    
    if (currentStatus.detected) {
        currentStatus.detectionTime = now;
    }
    
    // Calculate approach velocity for prediction
    if (predictionEnabled && dt > 0) {
        int zoneIndex = (int)currentStatus.zone;
        if (zoneIndex < 4) {
            approachVelocity = calculateApproachVelocity(
                closestDistance, lastDistances[zoneIndex], dt
            );
            lastDistances[zoneIndex] = closestDistance;
            
            // Upgrade severity if approaching quickly
            if (approachVelocity < -200) {  // Approaching at >200mm/s
                if (currentStatus.severity == SEVERITY_WARNING) {
                    currentStatus.severity = SEVERITY_CRITICAL;
                } else if (currentStatus.severity == SEVERITY_CRITICAL) {
                    currentStatus.severity = SEVERITY_IMMINENT;
                }
            }
        }
    }
}

CollisionSeverity CollisionDetector::evaluateDistance(float distance) {
    if (distance < 0 || distance > 4000) {
        return SEVERITY_NONE;
    }
    
    if (distance < imminentDistance) {
        return SEVERITY_IMMINENT;
    } else if (distance < criticalDistance) {
        return SEVERITY_CRITICAL;
    } else if (distance < warningDistance) {
        return SEVERITY_WARNING;
    }
    
    return SEVERITY_NONE;
}

CollisionZone CollisionDetector::findClosestZone(float front, float back, 
                                                 float left, float right) {
    float minDist = front;
    CollisionZone zone = ZONE_FRONT;
    
    if (back > 0 && back < minDist) {
        minDist = back;
        zone = ZONE_BACK;
    }
    if (left > 0 && left < minDist) {
        minDist = left;
        zone = ZONE_LEFT;
    }
    if (right > 0 && right < minDist) {
        minDist = right;
        zone = ZONE_RIGHT;
    }
    
    return zone;
}

float CollisionDetector::calculateApproachVelocity(float currentDist, 
                                                   float lastDist, float dt) {
    if (dt <= 0) return 0;
    return (currentDist - lastDist) / dt;  // mm/s (negative = approaching)
}

void CollisionDetector::setThresholds(float warning, float critical, float imminent) {
    warningDistance = warning;
    criticalDistance = critical;
    imminentDistance = imminent;
}