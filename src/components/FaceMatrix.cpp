#include "FaceMatrix.h"

// Neo Matrix Constants
#define NEO_MATRIX_DATA_PIN 1
#define NEO_MATRIX_WIDTH 48
#define NEO_MATRIX_HEIGHT 8

#define NEO_MATRIX_BRIGHTNESS_MULTIPLIER 3 // Brightness can be changed from 1-10 times this number

// Face Constants
#define FACE_WIDTH 16
#define FACE_HEIGHT 8

#define EYE_WIDTH 8
#define EYE_HEIGHT 8


FaceMatrix::FaceMatrix(int initialBrightness) {
    matrix = new Adafruit_NeoMatrix(NEO_MATRIX_WIDTH, NEO_MATRIX_HEIGHT, NEO_MATRIX_DATA_PIN,
        NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
        NEO_GRB + NEO_KHZ800);

    brightness = initialBrightness;
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

void FaceMatrix::display(uint16_t color, vector<Point>& eyeVector, vector<Point>& mouthVector) {

    matrix->fillScreen(0);

    // Left eye
    for (double y = 0; y < EYE_HEIGHT; y++) {
        for (double x = 0; x < EYE_WIDTH; x++) {
            uint8_t brightness = getPixelBrightness({x, y}, eyeVector);
            if (brightness > 0) {
                matrix->drawPixel(x, y, matrix->Color(0, 0, brightness));
            }
            // if (pointInPolygon(eyeVector, {x, y})) {
            //     matrix->drawPixel(x, y, color);
            // }
        }
    }

    matrix->show();
}

uint8_t FaceMatrix::getPixelBrightness(Point point, vector<Point>& vector) {
    uint8_t hits = 0;
    Point p = {0.0, 0.0};
    for (p.y = point.y+0.0; p.y < point.y+1; p.y += (1.0/8)) {
        for (p.x = point.x+0.0; p.x < point.x+1; p.x += (1.0/8)) {
            if (pointInPolygon(vector, p)) {
                hits++;
            }
        }
    }
    return floor(hits*(255.0/64.0));
}

void FaceMatrix::setBrightness(int newBrightness) {
    brightness = newBrightness;
    matrix->setBrightness(brightness * NEO_MATRIX_BRIGHTNESS_MULTIPLIER);
}