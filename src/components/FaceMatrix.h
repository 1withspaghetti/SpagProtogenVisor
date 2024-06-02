#include "definitions/FaceVectors.h"
#include "util/VectorUtil.h"
#include <FastLED.h>

using namespace std;

// Neo Matrix Constants
#define NEO_MATRIX_DATA_PIN 1
#define NEO_MATRIX_WIDTH 48
#define NEO_MATRIX_HEIGHT 8

// Brightness Constants
#define FACE_BRIGHTNESS_INITIAL 10 // Brightness can be changed from 1-10 times this number
#define FACE_BRIGHTNESS_MULTIPLIER 12 // Brightness can be changed from 1-10 times this number

// Vector Display Constants
#define ESTIMATE_WIDTH 4
#define ESTIMATE_HEIGHT 4


class FaceMatrix {
    private:
        CLEDController* ledController;
        CRGB leds[NEO_MATRIX_WIDTH * NEO_MATRIX_HEIGHT];
        uint8_t coverageCache[NEO_MATRIX_WIDTH * NEO_MATRIX_HEIGHT];
        vector<Point> coverageCacheEyeVector;
        vector<Point> coverageCacheMouthVector;

        uint8_t brightness;
        uint8_t getPixelCoverage(Point p, vector<Point>& vector);
        uint16_t getPixelIndex(uint8_t x, uint8_t y);
    public:
        FaceMatrix(uint8_t initialBrightness = 10);
        ~FaceMatrix();
        void setup();
        /**
         * Display the face on the matrix, will use a cached version of the coverage vector to speed up the process if possible
         * 
         * @return true if the coverage vector was recalculated, false if the cached version was used
        */
        bool display(CRGB color, vector<Point>& eyeVector, vector<Point>& mouthVector);
        /**
         * Render the face on the matrix, will not use a cached version of the coverage vector in stead it will calculate it every time
        */
        void render(CRGB color, vector<Point>& eyeVector, vector<Point>& mouthVector);
        void setBrightness(uint8_t newBrightness);
};