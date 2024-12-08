#include "definitions/FaceVectors.h"
#include "util/VectorUtil.h"
#include <FastLED.h>

using namespace std;

// Neo Matrix Constants
#define NEO_MATRIX_DATA_PIN 1
#define NEO_MATRIX_WIDTH 48
#define NEO_MATRIX_HEIGHT 8

// Vector Coverage Estimation Constants
// The estimate is used to speed up the coverage calculation by only checking a few points in the pixel
// Visual pixel size about follows the function 0.5^{2}=\left(x-0.5\right)^{2}+\left(y+0.5\right)^{2}
// The things before define a box to check relative to a point, with its start and end points and how often to check
#define ESTIMATE_START_X 0.2
#define ESTIMATE_INCREMENT_X 0.4
#define ESTIMATE_END_X 0.8
#define ESTIMATE_START_Y 0.2
#define ESTIMATE_INCREMENT_Y 0.4
#define ESTIMATE_END_Y 0.8
#define ESTIMATE_TOTAL_ATTEMPTS ((ESTIMATE_END_X - ESTIMATE_START_X) / ESTIMATE_INCREMENT_X) * ((ESTIMATE_END_Y - ESTIMATE_START_Y) / ESTIMATE_INCREMENT_Y) + 4 // 4 is the amount of points checked in the initial optimization


class FaceMatrix {
    private:
        CLEDController* ledController;
        CRGB leds[NEO_MATRIX_WIDTH * NEO_MATRIX_HEIGHT];
        uint8_t coverageCache[NEO_MATRIX_WIDTH * NEO_MATRIX_HEIGHT];
        vector<Point> coverageCacheEyeVector;
        vector<Point> coverageCacheMouthVector;

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
        bool display(CRGB color, uint8_t brightness, vector<Point>& eyeVector, vector<Point>& mouthVector, vector<Point>& specialVector);
        /**
         * Render the face on the matrix, will not use a cached version of the coverage vector in stead it will calculate it every time
        */
        void getMatrixCoverage(CRGB color, uint8_t brightness, vector<Point>& eyeVector, vector<Point>& mouthVector);
};