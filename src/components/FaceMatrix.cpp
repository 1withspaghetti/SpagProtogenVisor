#include "FaceMatrix.h"

#define FACE_BRIGHTNESS_INITIAL 16 // Brightness can be changed from 1-10 times this number
#define FACE_BRIGHTNESS_MULTIPLIER 4 // Brightness can be changed from 1-10 times this number

// Face Constants
#define FACE_WIDTH 16
#define FACE_HEIGHT 8

#define EYE_WIDTH 8
#define EYE_HEIGHT 8

// Vector Display Constants
#define ESTIMATE_WIDTH 4
#define ESTIMATE_HEIGHT 4


FaceMatrix::FaceMatrix(uint8_t initialBrightness) {
    ledController = &FastLED.addLeds<NEOPIXEL, NEO_MATRIX_DATA_PIN>(leds, NEO_MATRIX_WIDTH * NEO_MATRIX_HEIGHT).setCorrection(TypicalLEDStrip);
    brightness = initialBrightness * FACE_BRIGHTNESS_MULTIPLIER + FACE_BRIGHTNESS_INITIAL;
}

FaceMatrix::~FaceMatrix() {
    ledController->clearLedData();
    ledController = nullptr;
}

void FaceMatrix::setup() {
    ledController->setDither(0);
    ledController->showColor(CRGB::Black, 0);
}

bool FaceMatrix::display(CRGB color, vector<Point>& eyeVector, vector<Point>& mouthVector) {
    if (coverageCacheEyeVector != eyeVector || coverageCacheMouthVector != mouthVector) {

        Serial.println("FaceMatrix: Recalculating coverage vectors");

        // Recalculate the coverage
        render(color, eyeVector, mouthVector);
        coverageCacheEyeVector = eyeVector;
        coverageCacheMouthVector = mouthVector;

        return true;

    } else {
        
        // Use the cached version of the coverage vectors
        ledController->clearLedData();
        for (int i = 0; i < NEO_MATRIX_WIDTH * NEO_MATRIX_HEIGHT; i++) {
            leds[i] = CRGB(color).nscale8(coverageCache[i]);
        }
        ledController->showLeds(brightness);

        return false;
    }
}

void FaceMatrix::render(CRGB color, vector<Point>& eyeVector, vector<Point>& mouthVector) {

    // Clear the matrix
    ledController->clearLedData();

    // Left eye
    for (double y = 0; y < EYE_HEIGHT; y++) {
        for (double x = 0; x < EYE_WIDTH; x++) {
            uint16_t index = getPixelIndex(x, y);
            uint8_t cover = getPixelCoverage({x, y}, eyeVector);
            coverageCache[index] = cover;
            if (cover > 0) leds[index] = CRGB(color).nscale8(cover);
        }
    }

    // Right eye
    for (double y = 0; y < EYE_HEIGHT; y++) {
        for (double x = 0; x < EYE_WIDTH; x++) {
            uint16_t index = getPixelIndex(NEO_MATRIX_WIDTH - 1 - x, y);
            uint8_t cover = getPixelCoverage({x, y}, eyeVector);
            coverageCache[index] = cover;
            if (cover > 0) leds[index] = CRGB(color).nscale8(cover);
        }
    }

    // Left Mouth
    for (double y = 4; y < FACE_HEIGHT; y++) { // There is a optimization here that assumes the mouth is always in the bottom half of the face
        for (double x = 0; x < FACE_WIDTH; x++) {
            uint16_t index = getPixelIndex(x + EYE_WIDTH, y);
            uint8_t cover = getPixelCoverage({x, y}, mouthVector);
            coverageCache[index] = cover;
            if (cover > 0) leds[index] = CRGB(color).nscale8(cover);
        }
    }

    // Right Mouth
    for (double y = 4; y < FACE_HEIGHT; y++) { // There is a optimization here that assumes the mouth is always in the bottom half of the face
        for (double x = 0; x < FACE_WIDTH; x++) {
            uint16_t index = getPixelIndex(NEO_MATRIX_WIDTH - 1 - x - EYE_WIDTH, y);
            uint8_t cover = getPixelCoverage({x, y}, mouthVector);
            coverageCache[index] = cover;
            if (cover > 0) leds[index] = CRGB(color).nscale8(cover);
        }
    }

    ledController->showLeds(brightness);
}

/**
 * It is used to get the led arr index at a given x and y coordinates
 * The x is flipped to account for x input being from the left side of the matrix
 * It also accounts for the column zigzag pattern of the matrix
*/
uint16_t FaceMatrix::getPixelIndex(uint8_t x, uint8_t y) {
    return (
        NEO_MATRIX_HEIGHT * (NEO_MATRIX_WIDTH - 1 - x) +
        ((NEO_MATRIX_WIDTH - 1 - x) % 2 == 1 ? y : NEO_MATRIX_HEIGHT - 1 - y)
    );
}

uint8_t FaceMatrix::getPixelCoverage(Point point, vector<Point>& vector) {
    uint8_t hits = 0;

    // For optimization, check corners of the pixel first
    if (pointInPolygon(vector, {point.x+0.0, point.y+0.0})) hits++;
    if (pointInPolygon(vector, {point.x+1.0, point.y+0.0})) hits++;
    if (pointInPolygon(vector, {point.x+0.0, point.y+1.0})) hits++;
    if (pointInPolygon(vector, {point.x+1.0, point.y+1.0})) hits++;

    if (hits == 0) return 0;
    else if (hits == 4) return 255;

    // Check the rest of the pixel for more accurate brightness
    hits = 0;

    Point p = {0.0, 0.0};
    for (p.y = point.y+0.0; p.y < point.y+1; p.y += (1.0/ESTIMATE_HEIGHT)) {
        for (p.x = point.x+0.0; p.x < point.x+1; p.x += (1.0/ESTIMATE_WIDTH)) {
            if (pointInPolygon(vector, p)) {
                hits++;
            }
        }
    }
    return floor(hits*(255.0/(ESTIMATE_WIDTH*ESTIMATE_HEIGHT)));
}

void FaceMatrix::setBrightness(uint8_t newBrightness) {
    brightness = newBrightness * FACE_BRIGHTNESS_MULTIPLIER + FACE_BRIGHTNESS_INITIAL;
}