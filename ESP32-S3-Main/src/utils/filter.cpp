#include "filter.h"

// Moving Average Filter Implementation
MovingAverageFilter::MovingAverageFilter(int filterSize) 
    : size(filterSize), index(0), count(0), sum(0) {
    values = new float[size];
    for (int i = 0; i < size; i++) {
        values[i] = 0;
    }
}

MovingAverageFilter::~MovingAverageFilter() {
    delete[] values;
}

float MovingAverageFilter::update(float newValue) {
    // Subtract old value from sum
    sum -= values[index];
    
    // Add new value
    values[index] = newValue;
    sum += newValue;
    
    // Update index
    index = (index + 1) % size;
    
    // Track count for initial fill
    if (count < size) count++;
    
    return sum / count;
}

void MovingAverageFilter::reset() {
    for (int i = 0; i < size; i++) {
        values[i] = 0;
    }
    sum = 0;
    count = 0;
    index = 0;
}

float MovingAverageFilter::getAverage() {
    return (count > 0) ? sum / count : 0;
}

// EMA Filter Implementation
EMAFilter::EMAFilter(float smoothing) 
    : alpha(smoothing), value(0), initialized(false) {}

float EMAFilter::update(float newValue) {
    if (!initialized) {
        value = newValue;
        initialized = true;
    } else {
        value = alpha * newValue + (1 - alpha) * value;
    }
    return value;
}

void EMAFilter::reset() {
    value = 0;
    initialized = false;
}

// Kalman Filter Implementation
KalmanFilter::KalmanFilter(float processNoise, float measurementNoise)
    : q(processNoise), r(measurementNoise), x(0), p(1), k(0) {}

float KalmanFilter::update(float measurement) {
    // Prediction
    p = p + q;
    
    // Update
    k = p / (p + r);
    x = x + k * (measurement - x);
    p = (1 - k) * p;
    
    return x;
}

void KalmanFilter::reset() {
    x = 0;
    p = 1;
    k = 0;
}

// Median Filter Implementation
MedianFilter::MedianFilter(int filterSize) 
    : size(filterSize), index(0) {
    values = new float[size];
    for (int i = 0; i < size; i++) {
        values[i] = 0;
    }
}

MedianFilter::~MedianFilter() {
    delete[] values;
}

float MedianFilter::update(float newValue) {
    values[index] = newValue;
    index = (index + 1) % size;
    return getMedian();
}

float MedianFilter::getMedian() {
    // Create a copy and sort
    float sorted[size];
    for (int i = 0; i < size; i++) {
        sorted[i] = values[i];
    }
    
    // Simple bubble sort
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (sorted[j] > sorted[j + 1]) {
                float temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }
    
    // Return median
    return sorted[size / 2];
}

void MedianFilter::reset() {
    for (int i = 0; i < size; i++) {
        values[i] = 0;
    }
    index = 0;
}