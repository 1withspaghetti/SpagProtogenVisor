#include "EmotionManager.h"


EmotionManager::EmotionManager(int initialEmotion) {
    emotion = initialEmotion;
    resetBlinkTimer();
}

EmotionManager::~EmotionManager()
{
    eyeVector.clear();
    mouthVector.clear();
}

void EmotionManager::setup() {
    // Expand from bottom right corner
    eyeVector.push_back(Point(EYE_WIDTH, EYE_HEIGHT));
    mouthVector.push_back(Point(FACE_WIDTH, FACE_HEIGHT));
}

void EmotionManager::tick(double mic_magnitude) {
    if (mic_magnitude < MIC_SCALE_THRESHOLD) mic_magnitude = 0;
    if (mic_magnitude > MIC_SCALE_MAX) mic_magnitude = MIC_SCALE_MAX;

    blinkTimer--;
    if (blinkTimer < 0) resetBlinkTimer();

    vector<Point> targetEyeVector = FaceVectors::getEyeVector(emotion); // Create a copy to manipulate
    transformVector(targetEyeVector, 
                    EYE_TRANSFORM_SRC_X, 
                    EYE_TRANSFORM_SRC_Y, 
                    1, 
                    blinkTimer < BLINK_DURATION ? 0.2 : 1);
    interpolateVector(eyeVector, targetEyeVector, 0.25, 0.25);


    vector<Point> targetMouthVector = FaceVectors::getMouthVector(emotion);
    transformVector(targetMouthVector, 
                    MOUTH_TRANSFORM_SRC_X, 
                    MOUTH_TRANSFORM_SRC_Y, 
                    1 + mic_magnitude, 
                    1 + (mic_magnitude * 1.5));
    interpolateVector(mouthVector, targetMouthVector, 0.25, 0.25);
}

void EmotionManager::setEmotion(int newEmotion) {
    Serial.println("Setting emotion to " + String(newEmotion));
    emotion = newEmotion;
}

int EmotionManager::getEmotion() {
    return emotion;
}

vector<Point>& EmotionManager::getEyeVector() {
    return eyeVector;
}

vector<Point>& EmotionManager::getMouthVector() {
    return mouthVector;
}

/**
 * Get the current blink timer value. The blink happens when the value is less than BLINK_DURATION and resets at 0.
 * @return Ticks before the next blink reset
 */
int EmotionManager::getBlinkTimer() {
    return blinkTimer;
}

/**
 * Set the blink timer to a new value. The blink happens when the value is less than BLINK_DURATION and resets at 0.
 * @param newBlinkTimer Ticks before the next blink reset, cannot be negative
 */
void EmotionManager::setBlinkTimer(int newBlinkTimer) {
    blinkTimer = max(newBlinkTimer, 0);
}

/**
 * Start a blink by setting the blink timer to BLINK_DURATION
 */
void EmotionManager::startBlink() {
    blinkTimer = BLINK_DURATION;
}

/**
 * Reset the blink timer to a random value between BLINK_INTERVAL_MIN and BLINK_INTERVAL_MAX
 */
void EmotionManager::resetBlinkTimer() {
    blinkTimer = random(BLINK_INTERVAL_MIN, BLINK_INTERVAL_MAX);
}