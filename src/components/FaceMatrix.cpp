#include "FaceMatrix.h"

#define NEO_MATRIX_BRIGHTNESS_MULTIPLIER 4 // Brightness can be changed from 1-10 times this number

// Face Constants
#define FACE_WIDTH 16
#define FACE_HEIGHT 8

#define EYE_WIDTH 8
#define EYE_HEIGHT 8

// Vector Display Constants
#define ESTIMATE_WIDTH 4
#define ESTIMATE_HEIGHT 4


FaceMatrix::FaceMatrix(int initialBrightness) {
    FastLED.addLeds<NEOPIXEL, NEO_MATRIX_DATA_PIN>(leds, NEO_MATRIX_WIDTH * NEO_MATRIX_HEIGHT).setCorrection(TypicalLEDStrip);
    setBrightness(initialBrightness);
}

FaceMatrix::~FaceMatrix() {
    FastLED.clear();
    FastLED.show();
}

void FaceMatrix::setup() {
    delay(1000);
    setBrightness(brightness);
}

void FaceMatrix::display(CRGB color, vector<Point>& eyeVector, vector<Point>& mouthVector) {

    // Clear the matrix
    FastLED.clear();

    // Left eye
    for (double y = 0; y < EYE_HEIGHT; y++) {
        for (double x = 0; x < EYE_WIDTH; x++) {
            uint8_t cover = getPixelBrightness({x, y}, eyeVector);
            if (cover > 0) {
                setPixel(x, y, CRGB(color).nscale8(cover));
            }
        }
    }

    // Right eye
    for (double y = 0; y < EYE_HEIGHT; y++) {
        for (double x = 0; x < EYE_WIDTH; x++) {
            uint8_t cover = getPixelBrightness({x, y}, eyeVector);
            if (cover > 0) {
                setPixel(NEO_MATRIX_WIDTH - 1 - x, y, CRGB(color).nscale8(cover));
            }
        }
    }

    // Left Mouth
    for (double y = 4; y < FACE_HEIGHT; y++) { // There is a optimization here that assumes the mouth is always in the bottom half of the face
        for (double x = 0; x < FACE_WIDTH; x++) {
            uint8_t cover = getPixelBrightness({x, y}, mouthVector);
            if (cover > 0) {
                setPixel(x + EYE_WIDTH, y, CRGB(color).nscale8(cover));
            }
        }
    }

    // Right Mouth
    for (double y = 4; y < FACE_HEIGHT; y++) { // There is a optimization here that assumes the mouth is always in the bottom half of the face
        for (double x = 0; x < FACE_WIDTH; x++) {
            uint8_t cover = getPixelBrightness({x, y}, mouthVector);
            if (cover > 0) {
                setPixel(NEO_MATRIX_WIDTH - EYE_WIDTH - 1 - x, y, CRGB(color).nscale8(cover));
            }
        }
    }


    FastLED.show();
}

void FaceMatrix::setPixel(uint8_t x, uint8_t y, CRGB color) {
    // This function is used to set a pixel on the matrix
    // It is used to set the pixel at the given x and y coordinates to the given color
    // The x is flipped to account for x input being from the left side of the matrix
    // It also accounts for the column zigzag pattern of the matrix
    leds[
        NEO_MATRIX_HEIGHT * (NEO_MATRIX_WIDTH - 1 - x) +
        ((NEO_MATRIX_WIDTH - 1 - x) % 2 == 1 ? y : NEO_MATRIX_HEIGHT - 1 - y)
    ] = color;
}

uint8_t FaceMatrix::getPixelBrightness(Point point, vector<Point>& vector) {
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

void FaceMatrix::setBrightness(int newBrightness) {
    brightness = newBrightness;
    FastLED.setBrightness(brightness * NEO_MATRIX_BRIGHTNESS_MULTIPLIER);
}