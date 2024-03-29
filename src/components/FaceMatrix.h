#include <Adafruit_NeoMatrix.h>
#include "definitions/FaceVectors.h"
#include "util/VectorUtil.h"
using namespace std;

class FaceMatrix {
    private:
        Adafruit_NeoMatrix *matrix;
        int brightness;

        vector<Point> eyeVector;
        vector<Point> mouthVector;
    public:
        FaceMatrix(int initialEmotion = 0, int initialBrightness = 10);
        ~FaceMatrix();
        void setup();
        void tick(int targetEmotion);
        void display(uint16_t color);
        void setBrightness(int newBrightness);
};