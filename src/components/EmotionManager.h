#include "definitions/FaceVectors.h"
#include "util/VectorUtil.h"
using namespace std;

// Max time between blinks in ticks
#define BLINK_INTERVAL_MAX 240 // 12 seconds
// Min time between blinks in ticks
#define BLINK_INTERVAL_MIN 120 // 6 seconds
// Duration of blink in ticks
#define BLINK_DURATION 5 // 0.25 seconds

// Source point for transforming the eye for blinking
#define EYE_TRANSFORM_SRC_X 3
#define EYE_TRANSFORM_SRC_Y 6

// Source point for transforming the mouth for speaking
#define MOUTH_TRANSFORM_SRC_X 10
#define MOUTH_TRANSFORM_SRC_Y 8

// Magnitude is a double from 0 to 1
#define MIC_SCALE_THRESHOLD 0.15 // Minimum magnitude to consider
#define MIC_SCALE_MAX 0.5 // Maximum magnitude

class EmotionManager {
    private:
        int emotion;
        vector<Point> eyeVector;
        vector<Point> mouthVector;
        int blinkTimer;
    public:
        EmotionManager(int initialEmotion = 0);
        ~EmotionManager();
        void setup();
        void tick(double mic_magnitude);
        void setEmotion(int newEmotion);
        int getEmotion();
        vector<Point>& getEyeVector();
        vector<Point>& getMouthVector();
        int getBlinkTimer();
        void setBlinkTimer(int newBlinkTimer);
        void startBlink();
        void resetBlinkTimer();
};