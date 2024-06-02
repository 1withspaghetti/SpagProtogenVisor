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

void EmotionManager::tick() {
    vector<Point> targetEyeVector = FaceVectors::getEyeVector(emotion);
    interpolateVector(eyeVector, targetEyeVector, 2);

    vector<Point> targetMouthVector = FaceVectors::getMouthVector(emotion);
    interpolateVector(mouthVector, targetMouthVector, 2);
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