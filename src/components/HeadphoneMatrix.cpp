#include "HeadphoneMatrix.h"

#define HEADPHONE_NEO_MATRIX_DATA_PIN D3
#define HEADPHONE_NEO_MATRIX_WIDTH 16
#define HEADPHONE_NEO_MATRIX_HEIGHT 8

#define HEADPHONE_NEO_MATRIX_BRIGHTNESS 7

HeadphoneMatrix::HeadphoneMatrix() {
    matrix = new Adafruit_NeoMatrix(HEADPHONE_NEO_MATRIX_WIDTH, HEADPHONE_NEO_MATRIX_HEIGHT, HEADPHONE_NEO_MATRIX_DATA_PIN,
        NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
        NEO_GRB + NEO_KHZ800);
}

HeadphoneMatrix::~HeadphoneMatrix()
{
    delete matrix;
}

void HeadphoneMatrix::setup() {
    matrix->begin();
    matrix->setBrightness(HEADPHONE_NEO_MATRIX_BRIGHTNESS);
}

void HeadphoneMatrix::display(uint16_t color) {
    matrix->fillScreen(0);
    for (int y = 0; y < HEADPHONE_HEIGHT; y++) {
        uint8_t row = pgm_read_byte(&(headphone_ico[y]));
        for (int x = 0; x < HEADPHONE_WIDTH; x++) {
            if (row >> (HEADPHONE_WIDTH-1 - x) & 0b1 == 1) {
                matrix->drawPixel(x, y, color);
                matrix->drawPixel(x + HEADPHONE_WIDTH, y, color);
            }
        }
    }
    matrix->show();
}