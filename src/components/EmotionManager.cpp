#include "EmotionManager.h"


EmotionManager::EmotionManager(int initialEmotion) {
    emotion = initialEmotion;
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
    if (mic_magnitude < MIC_MAGNITUDE_THRESHOLD) mic_magnitude = 0;
    if (mic_magnitude > MIC_MAGNITUDE_MAX) mic_magnitude = MIC_MAGNITUDE_MAX;

    const vector<Point> targetEyeVector = FaceVectors::getEyeVector(emotion);
    interpolateVector(eyeVector, targetEyeVector, 0.25, 0.25);


    const vector<Point> targetMouthVector = FaceVectors::getMouthVector(emotion);

    std::vector<Point> temp(targetMouthVector.size(), Point(0, 0));
    transformVector(targetMouthVector, temp, MOUTH_TRANSFORM_SRC_X, MOUTH_TRANSFORM_SRC_Y, 1 + mic_magnitude);

    interpolateVector(mouthVector, temp, 0.25, 0.25);
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