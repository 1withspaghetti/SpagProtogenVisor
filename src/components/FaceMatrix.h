#include "util/VectorUtil.h"
#include <FastLED.h>

using namespace std;

// Neo Matrix Constants
#define NEO_MATRIX_DATA_PIN 1
#define NEO_MATRIX_WIDTH 48
#define NEO_MATRIX_HEIGHT 8

class FaceMatrix {
    private:
        CRGB leds[NEO_MATRIX_WIDTH * NEO_MATRIX_HEIGHT];
        int brightness;
        uint8_t getPixelBrightness(Point p, vector<Point>& vector);
        void setPixel(uint8_t x, uint8_t y, CRGB color);
    public:
        FaceMatrix(int initialBrightness = 10);
        ~FaceMatrix();
        void setup();
        void display(CRGB color, vector<Point>& eyeVector, vector<Point>& mouthVector);
        void setBrightness(int newBrightness);
};