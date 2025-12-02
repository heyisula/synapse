#ifndef FILTER_H
#define FILTER_H

#include <Arduino.h>

// Moving Average Filter
class MovingAverageFilter {
private:
    float* values;
    int size;
    int index;
    int count;
    float sum;
    
public:
    MovingAverageFilter(int filterSize = 5);
    ~MovingAverageFilter();
    
    float update(float newValue);
    void reset();
    float getAverage();
};

// Exponential Moving Average Filter
class EMAFilter {
private:
    float alpha;  // Smoothing factor (0-1)
    float value;
    bool initialized;
    
public:
    EMAFilter(float smoothing = 0.3);
    
    float update(float newValue);
    void reset();
    float getValue() { return value; }
};

// Kalman Filter (simplified 1D)
class KalmanFilter {
private:
    float q;  // Process noise covariance
    float r;  // Measurement noise covariance
    float x;  // Estimated value
    float p;  // Estimation error covariance
    float k;  // Kalman gain
    
public:
    KalmanFilter(float processNoise = 0.01, float measurementNoise = 0.1);
    
    float update(float measurement);
    void reset();
    float getValue() { return x; }
};

// Median Filter
class MedianFilter {
private:
    float* values;
    int size;
    int index;
    
public:
    MedianFilter(int filterSize = 5);
    ~MedianFilter();
    
    float update(float newValue);
    void reset();
    
private:
    float getMedian();
};

#endif