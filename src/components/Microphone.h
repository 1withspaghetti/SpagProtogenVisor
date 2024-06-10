#include <Arduino.h>

#define MICROPHONE_PIN A7

#define SAMPLE_WINDOW_MILLIS 50 // Sample window width in mS (50 mS = 20Hz)

#define ROLLING_AVERAGE_SPLIT_TRIGGER 100 // If the # of samples exceeds this, remove the number of samples in ROLLING_AVERAGE_SPLIT_REMOVAL
#define ROLLING_AVERAGE_SPLIT_REMOVAL 50

#define MAGNITUDE_THRESHOLD 200

#define MAX_ANALOG_READ 4095

class Microphone {
    private:
        int sample;
        int sumForAverage;
        int countForAverage;
        volatile double magnitude;
    public:
        Microphone();
        ~Microphone();
        void setup();
        void tick();
        double getMagnitude();
};