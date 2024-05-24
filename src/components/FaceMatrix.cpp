#include "FaceMatrix.h"

// Neo Matrix Constants
#define NEO_MATRIX_DATA_PIN D4
#define NEO_MATRIX_WIDTH 48
#define NEO_MATRIX_HEIGHT 8

#define NEO_MATRIX_BRIGHTNESS_MULTIPLIER 3 // Brightness can be changed from 1-10 times this number


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

    // Left eye ()
    Serial.println("Left Eye");
    matrix->fillRect(0, 0, 8, 8, color);

    matrix->show();
}

int8_t FaceMatrix::getPixelBrightness(Point point, vector<Point>& vector) {
    int8_t hits = 0;
    Point p = {0, 0};
    for (double y = point.x+0.0; y < point.x+1; y += (1/8)) {
        for (double x = point.y+0.0; x < point.y+1; x += (1/8)) {
            p.x = x;
            p.y = y;
            if (pointInPolygon(vector, p)) {
                hits++;
            }
        }
    }
    return hits*(255.0/64);
}

void FaceMatrix::setBrightness(int newBrightness) {
    brightness = newBrightness;
    matrix->setBrightness(brightness * NEO_MATRIX_BRIGHTNESS_MULTIPLIER);
}