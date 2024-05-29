#include "EmotionManager.h"


EmotionManager::EmotionManager(int initialEmotion) {
    emotion = initialEmotion;

    vector<Point> initialEyeVector = FaceVectors::getEyeVector(emotion);
    for (int i = 0; i < initialEyeVector.size(); i++) {
        eyeVector.push_back(initialEyeVector[i]);
    }

    vector<Point> initialMouthVector = FaceVectors::getMouthVector(emotion);
    for (int i = 0; i < initialMouthVector.size(); i++) {
        mouthVector.push_back(initialMouthVector[i]);
    }
}

EmotionManager::~EmotionManager()
{
    eyeVector.clear();
    mouthVector.clear();
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