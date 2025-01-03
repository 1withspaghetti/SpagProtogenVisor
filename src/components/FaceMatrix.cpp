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
        #ifdef VERBOSE_RENDER_TIME
        Serial.println("FaceMatrix: Recalculating coverage vectors");
        #endif

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

    // Special vector (ix exists) is drawn on top of the mouth (and not cached)
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

    return hadToRecalculate; // Return if the coverage vector was recalculated, allows for further optimization
}

void FaceMatrix::getMatrixCoverage(CRGB color, uint8_t brightness, vector<Point>& eyeVector, vector<Point>& mouthVector) {

    // Eyes
    for (double y = 0; y < EYE_HEIGHT; y++) {
        for (double x = 0; x < EYE_WIDTH; x++) {
            // Get where the pixel would be in the left and right eye matrixes
            uint16_t index_left = getPixelIndex(x, y);
            uint16_t index_right = getPixelIndex(NEO_MATRIX_WIDTH - 1 - x, y);
            // Calculate how much of the pixel is covered by the eye
            uint8_t cover = getPixelCoverage({x, y}, eyeVector);
            // Cache the coverage num for both sides
            coverageCache[index_left] = cover;
            coverageCache[index_right] = cover;
        }
    }

    // Mouth
    for (double y = 3; y < FACE_HEIGHT; y++) { // There is a optimization here that assumes the mouth is always in the bottom part of the face
        for (double x = 0; x < FACE_WIDTH; x++) {
            // Get where the pixel would be in the left and right mouth matrixes
            uint16_t index_left = getPixelIndex(x + EYE_WIDTH, y);
            uint16_t index_right = getPixelIndex(NEO_MATRIX_WIDTH - 1 - x - EYE_WIDTH, y);
            // Calculate how much of the pixel is covered by the mouth
            uint8_t cover = getPixelCoverage({x, y}, mouthVector);
            // Cache the coverage num for both sides
            coverageCache[index_left] = cover;
            coverageCache[index_right] = cover;
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

/**
 * It is used to estimate how much of a pixel is covered by a vector, assuming the pixel is the shape of a circle
 * It first checks the top, left, right, and bottom of the pixel, then a few points in a grid pattern within the pixel
 * Returns a value from 0 to 255 representing the coverage, 0 being none and 255 being full
 */
uint8_t FaceMatrix::getPixelCoverage(Point point, vector<Point>& vector) {
    uint8_t hits = 0;

    // For optimization, check top center, left center, right center, bottom center (cause the pixel is a circle, so i'm not checking the corners)
    if (pointInPolygon(vector, {point.x+0.5, point.y+0.0})) hits++;
    if (pointInPolygon(vector, {point.x+1.0, point.y+0.5})) hits++;
    if (pointInPolygon(vector, {point.x+0.5, point.y+1.0})) hits++;
    if (pointInPolygon(vector, {point.x+0.0, point.y+0.5})) hits++;

    if (hits == 0) return 0;
    else if (hits == 4) return 255;

    // Now check a bunch of points within the pixel in a grid pattern
    Point p = {0.0, 0.0};
    for (p.y = point.y + ESTIMATE_START_Y; p.y < point.y + ESTIMATE_END_Y; p.y += ESTIMATE_INCREMENT_Y) {
        for (p.x = point.x + ESTIMATE_START_X; p.x < point.x + ESTIMATE_END_X; p.x += ESTIMATE_INCREMENT_X) {
            if (pointInPolygon(vector, p)) {
                hits++;
            }
        }
    }
    return floor(sqrt((double)hits/ESTIMATE_TOTAL_ATTEMPTS) * 255);
}
