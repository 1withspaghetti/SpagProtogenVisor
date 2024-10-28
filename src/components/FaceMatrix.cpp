#include "FaceMatrix.h"

FaceMatrix::FaceMatrix(uint8_t initialBrightness) {
    ledController = &FastLED.addLeds<NEOPIXEL, NEO_MATRIX_DATA_PIN>(leds, NEO_MATRIX_WIDTH * NEO_MATRIX_HEIGHT).setCorrection(TypicalLEDStrip);
}

FaceMatrix::~FaceMatrix() {
    ledController->clearLedData();
    ledController = nullptr;
}

void FaceMatrix::setup() {
    ledController->setDither(0);
    ledController->showColor(CRGB::Black, 0);
}

bool FaceMatrix::display(CRGB color, uint8_t brightness, vector<Point>& eyeVector, vector<Point>& mouthVector, vector<Point>& specialVector) {
    bool hadToRecalculate = false;
    if (coverageCacheEyeVector != eyeVector || coverageCacheMouthVector != mouthVector) {

        Serial.println("FaceMatrix: Recalculating coverage vectors");

        // Recalculate the coverage
        getMatrixCoverage(color, brightness, eyeVector, mouthVector);
        coverageCacheEyeVector = eyeVector;
        coverageCacheMouthVector = mouthVector;

        hadToRecalculate = true;
    }
        
    // Use the cached version of the coverage vectors
    ledController->clearLedData();
    for (int i = 0; i < NEO_MATRIX_WIDTH * NEO_MATRIX_HEIGHT; i++) {
        leds[i] = CRGB(color).nscale8(coverageCache[i]);
    }

    if (specialVector.size() > 0) {
        for (double y = 5; y < FACE_HEIGHT; y++) {
            for (double x = 0; x < FACE_WIDTH; x++) {
                uint16_t index = getPixelIndex(x + EYE_WIDTH, y);
                uint16_t indexMirror = getPixelIndex(NEO_MATRIX_WIDTH - 1 - x - EYE_WIDTH, y);
                uint8_t cover = getPixelCoverage({x, y}, specialVector);
                if (cover > 0) {
                    leds[index] = CRGB(CRGB::White).nscale8(cover/2);
                    leds[indexMirror] = CRGB(CRGB::White).nscale8(cover/2);
                }
            }
        }
    }


    ledController->showLeds(brightness);

    return hadToRecalculate;
}

void FaceMatrix::getMatrixCoverage(CRGB color, uint8_t brightness, vector<Point>& eyeVector, vector<Point>& mouthVector) {

    // Left eye
    for (double y = 0; y < EYE_HEIGHT; y++) {
        for (double x = 0; x < EYE_WIDTH; x++) {
            uint16_t index = getPixelIndex(x, y);
            uint8_t cover = getPixelCoverage({x, y}, eyeVector);
            coverageCache[index] = cover;
        }
    }

    // Right eye
    for (double y = 0; y < EYE_HEIGHT; y++) {
        for (double x = 0; x < EYE_WIDTH; x++) {
            uint16_t index = getPixelIndex(NEO_MATRIX_WIDTH - 1 - x, y);
            uint8_t cover = getPixelCoverage({x, y}, eyeVector);
            coverageCache[index] = cover;
        }
    }

    // Left Mouth
    for (double y = 3; y < FACE_HEIGHT; y++) { // There is a optimization here that assumes the mouth is always in the bottom part of the face
        for (double x = 0; x < FACE_WIDTH; x++) {
            uint16_t index = getPixelIndex(x + EYE_WIDTH, y);
            uint8_t cover = getPixelCoverage({x, y}, mouthVector);
            coverageCache[index] = cover;
        }
    }

    // Right Mouth
    for (double y = 3; y < FACE_HEIGHT; y++) { // There is a optimization here that assumes the mouth is always in the bottom part of the face
        for (double x = 0; x < FACE_WIDTH; x++) {
            uint16_t index = getPixelIndex(NEO_MATRIX_WIDTH - 1 - x - EYE_WIDTH, y);
            uint8_t cover = getPixelCoverage({x, y}, mouthVector);
            coverageCache[index] = cover;
        }
    }
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
