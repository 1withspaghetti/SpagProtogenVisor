#include "definitions/FaceVectors.h"
#include "util/VectorUtil.h"
using namespace std;

#define MOUTH_TRANSFORM_SRC_X 10
#define MOUTH_TRANSFORM_SRC_Y 8

// Magnitude is a double from 0 to 1
#define MIC_MAGNITUDE_THRESHOLD 0.15 // Minimum magnitude to consider
#define MIC_MAGNITUDE_MAX 0.5 // Maximum magnitude

class EmotionManager {
    private:
        int emotion;
        vector<Point> eyeVector;
        vector<Point> mouthVector;
    public:
        EmotionManager(int initialEmotion = 0);
        ~EmotionManager();
        void setup();
        void tick(double mic_magnitude);
        void setEmotion(int newEmotion);
        int getEmotion();
        vector<Point>& getEyeVector();
        vector<Point>& getMouthVector();
};