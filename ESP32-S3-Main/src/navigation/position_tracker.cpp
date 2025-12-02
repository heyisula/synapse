#include "position_tracker.h"
#include <math.h>

PositionTracker::PositionTracker() 
    : distanceTraveled(0), lastUpdateTime(0), 
      confidenceDecayRate(0.01), lastLineDetection(0) {
    position.x = 0;
    position.y = 0;
    position.heading = 0;
    position.confidence = 0.5;
}

void PositionTracker::begin() {
    resetPosition();
    lastUpdateTime = millis();
}

void PositionTracker::update(float deltaDistance, float deltaHeading) {
    unsigned long now = millis();
    float dt = (now - lastUpdateTime) / 1000.0;
    lastUpdateTime = now;
    
    // Update heading
    position.heading += deltaHeading;
    
    // Wrap heading to 0-360
    while (position.heading > 360) position.heading -= 360;
    while (position.heading < 0) position.heading += 360;
    
    // Update position based on heading
    float headingRad = position.heading * PI / 180.0;
    position.x += deltaDistance * cos(headingRad);
    position.y += deltaDistance * sin(headingRad);
    
    // Track total distance
    distanceTraveled += abs(deltaDistance);
    
    // Decay confidence over time/distance
    decayConfidence();
}

void PositionTracker::setPosition(float x, float y, float heading) {
    position.x = x;
    position.y = y;
    position.heading = heading;
    position.confidence = 1.0;
    
    lastKnownGood = position;
    
    Serial.print("Position updated: (");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.print(") heading ");
    Serial.println(heading);
}

void PositionTracker::resetPosition() {
    position.x = 0;
    position.y = 0;
    position.heading = 0;
    position.confidence = 0.5;
    distanceTraveled = 0;
}

void PositionTracker::updateFromLine(int linePosition) {
    // Line detection improves confidence
    lastLineDetection = millis();
    position.confidence = min(1.0f, position.confidence + 0.1f);
    
    // Line position can help correct lateral offset
    // (Implementation depends on line sensor placement)
}

void PositionTracker::updateFromWaypoint(float x, float y) {
    // Reached a known waypoint - reset position
    setPosition(x, y, position.heading);
    distanceTraveled = 0;
    
    Serial.println("Position corrected at waypoint");
}

void PositionTracker::decayConfidence() {
    // Confidence decays with distance traveled
    float decay = distanceTraveled * confidenceDecayRate;
    position.confidence = max(0.0f, position.confidence - decay);
    
    // Also decay over time without line detection
    if (millis() - lastLineDetection > 10000) {  // 10 seconds
        position.confidence = max(0.0f, position.confidence - 0.01f);
    }
}