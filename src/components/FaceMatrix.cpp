#include "FaceMatrix.h"

// Neo Matrix Constants
#define NEO_MATRIX_DATA_PIN 3
#define NEO_MATRIX_WIDTH 48
#define NEO_MATRIX_HEIGHT 8

#define NEO_MATRIX_BRIGHTNESS_MULTIPLIER 3 // Brightness can be changed from 1-10 times this number


FaceMatrix::FaceMatrix(int initialEmotion, int initialBrightness) {
    matrix = new Adafruit_NeoMatrix(NEO_MATRIX_WIDTH, NEO_MATRIX_HEIGHT, NEO_MATRIX_DATA_PIN,
        NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
        NEO_GRB + NEO_KHZ800);

    brightness = initialBrightness;

    vector<Point> initialEyeVector = FaceVectors::getEyeVector(initialEmotion);
    for (int i = 0; i < initialEyeVector.size(); i++) {
        eyeVector.push_back(initialEyeVector[i]);
    }

    vector<Point> initialMouthVector = FaceVectors::getMouthVector(initialEmotion);
    for (int i = 0; i < initialMouthVector.size(); i++) {
        mouthVector.push_back(initialMouthVector[i]);
    }
}

FaceMatrix::~FaceMatrix()
{
    delete matrix;
}

void FaceMatrix::setup() {
    matrix->begin();
    matrix->setTextWrap(false);
    matrix->setBrightness(brightness * NEO_MATRIX_BRIGHTNESS_MULTIPLIER);
}

void FaceMatrix::tick(int targetEmotion) {
    vector<Point> targetEyeVector = FaceVectors::getEyeVector(targetEmotion);
    interpolateVector(eyeVector, targetEyeVector, 0.1);

    vector<Point> targetMouthVector = FaceVectors::getMouthVector(targetEmotion);
    interpolateVector(mouthVector, targetMouthVector, 0.1);
}

void FaceMatrix::display(uint16_t color) {

    matrix->fillScreen(0);

    

    matrix->show();
}

void FaceMatrix::setBrightness(int newBrightness) {
    brightness = newBrightness;
    matrix->setBrightness(brightness * NEO_MATRIX_BRIGHTNESS_MULTIPLIER);
}