#include <FastLED.h>
#include <definitions/Icons.h>

#define HEADPHONE_NEO_MATRIX_DATA_PIN 2
#define HEADPHONE_NEO_MATRIX_WIDTH 16
#define HEADPHONE_NEO_MATRIX_HEIGHT 8

#define HEADPHONE_NEO_MATRIX_BRIGHTNESS 7

class HeadphoneMatrix {
    private:
        CLEDController* ledController;
        CRGB leds[HEADPHONE_NEO_MATRIX_WIDTH * HEADPHONE_NEO_MATRIX_HEIGHT];
        
        uint16_t getPixelIndex(uint8_t x, uint8_t y);
    public:
        HeadphoneMatrix();
        ~HeadphoneMatrix();
        void setup();
        void display(CRGB color, uint8_t brightness);
};