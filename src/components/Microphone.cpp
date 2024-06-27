#include "Microphone.h"

Microphone::Microphone() {
    sample = 0;
    sumForAverage = 0;
    countForAverage = 0;
    magnitude = 0;
}

Microphone::~Microphone() {
}

void Microphone::setup() {
    // Setup the microphone
    pinMode(MICROPHONE_PIN, INPUT);
}

void Microphone::tick() {
    unsigned long startMillis= millis();  // Start of sample window
    unsigned int peakToPeak = 0;   // peak-to-peak level

    unsigned int signalMax = 0;
    unsigned int signalMin = MAX_ANALOG_READ;

    // collect data for 50 mS
    while (millis() - startMillis < SAMPLE_WINDOW_MILLIS)
    {
        sample = analogRead(MICROPHONE_PIN);
        //Serial.println(sample);
        if (sample < MAX_ANALOG_READ)  // toss out spurious readings
        {
            if (sample > signalMax)
            {
                signalMax = sample;  // save just the max levels
            }
            else if (sample < signalMin)
            {
                signalMin = sample;  // save just the min levels
            }
        }
    }
    peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude


    sumForAverage += peakToPeak;
    countForAverage++;

    // If we have more than 100 samples, remove the oldest 50
    if (countForAverage >= ROLLING_AVERAGE_SPLIT_TRIGGER) {
        sumForAverage -= (double)ROLLING_AVERAGE_SPLIT_REMOVAL/countForAverage * sumForAverage;
        countForAverage -= ROLLING_AVERAGE_SPLIT_REMOVAL;
    }

    int diffFromAverage = peakToPeak - (sumForAverage/countForAverage);
    magnitude = constrain(diffFromAverage, 0, MAGNITUDE_THRESHOLD) / (double)MAGNITUDE_THRESHOLD;

    // Serial.print(peakToPeak);
    // Serial.print(" ");
    // Serial.println(sumForAverage/countForAverage);

    // Serial.println(magnitude);
}

double Microphone::getMagnitude() {
    return magnitude;
}
