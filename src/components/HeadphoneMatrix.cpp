#include "HeadphoneMatrix.h"

#define HEADPHONE_NEO_MATRIX_DATA_PIN 2
#define HEADPHONE_NEO_MATRIX_WIDTH 16
#define HEADPHONE_NEO_MATRIX_HEIGHT 8

#define HEADPHONE_NEO_MATRIX_BRIGHTNESS 7

HeadphoneMatrix::HeadphoneMatrix() {
    ledController = &FastLED.addLeds<NEOPIXEL, HEADPHONE_NEO_MATRIX_DATA_PIN>(leds, HEADPHONE_NEO_MATRIX_WIDTH * HEADPHONE_NEO_MATRIX_HEIGHT).setCorrection(TypicalLEDStrip);
}

HeadphoneMatrix::~HeadphoneMatrix() {
    ledController->clearLedData();
    ledController = nullptr;
}

void HeadphoneMatrix::setup() {
    ledController->setDither(0);
    ledController->showColor(CRGB::Black, 0);
}

void HeadphoneMatrix::display(CRGB color, uint8_t brightness) {
    ledController->clearLedData();

    for (int y = 0; y < HEADPHONE_HEIGHT; y++) {
        uint8_t row = pgm_read_byte(&(headphone_ico[y]));
        for (int x = 0; x < HEADPHONE_WIDTH; x++) {
            if (row >> (HEADPHONE_WIDTH-1 - x) & 0b1 == 1) {
                leds[getPixelIndex(x, y)] = color;
                leds[getPixelIndex(HEADPHONE_NEO_MATRIX_WIDTH - x - 1, y)] = color;
            }
        }
    }

    ledController->showLeds(brightness);
}

/**
 * It is used to get the led arr index at a given x and y coordinates
 * The x is flipped to account for x input being from the left side of the matrix
 * It also accounts for the column zigzag pattern of the matrix
*/
uint16_t HeadphoneMatrix::getPixelIndex(uint8_t x, uint8_t y) {
    return (
        HEADPHONE_NEO_MATRIX_HEIGHT * (HEADPHONE_NEO_MATRIX_WIDTH - 1 - x) +
        ((HEADPHONE_NEO_MATRIX_WIDTH - 1 - x) % 2 == 1 ? y : HEADPHONE_NEO_MATRIX_HEIGHT - 1 - y)
    );
}
