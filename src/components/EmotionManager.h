#include "definitions/FaceVectors.h"
#include "util/VectorUtil.h"
using namespace std;

class EmotionManager {
    private:
        int emotion;
        vector<Point> eyeVector;
        vector<Point> mouthVector;
    public:
        EmotionManager(int initialEmotion = 0);
        ~EmotionManager();
        void tick();
        void setEmotion(int newEmotion);
        int getEmotion();
        vector<Point>& getEyeVector();
        vector<Point>& getMouthVector();
};