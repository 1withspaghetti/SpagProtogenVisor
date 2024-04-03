#include <Adafruit_NeoMatrix.h>
#include "util/VectorUtil.h"
using namespace std;

class FaceMatrix {
    private:
        Adafruit_NeoMatrix *matrix;
        int brightness;
    public:
        FaceMatrix(int initialBrightness = 10);
        ~FaceMatrix();
        void setup();
        void display(uint16_t color, vector<Point>& eyeVector, vector<Point>& mouthVector);
        void setBrightness(int newBrightness);
};