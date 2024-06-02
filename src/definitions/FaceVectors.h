#include <Arduino.h>
#include <vector>
#include "../util/Point.h"
using namespace std;

#ifndef FACE_VECTORS_H
#define FACE_VECTORS_H

// Face Constants
#define FACE_WIDTH 16
#define FACE_HEIGHT 8

#define EYE_WIDTH 8
#define EYE_HEIGHT 8

class FaceVectors {
    public:
        static const vector<Point>& getEyeVector(int emotion);
        static const vector<Point>& getMouthVector(int emotion);

    private:
        static const vector<Point> eye_1;
        static const vector<Point> eye_2;
        static const vector<Point> eye_3;
        static const vector<Point> eye_4;
        static const vector<Point> eye_5;
        static const vector<Point> mouth_1;
        static const vector<Point> mouth_2;
        static const vector<Point> mouth_3;
};


// ==============================================================================
//                            EMOTION DEFINITIONS
// ==============================================================================

#define EMOTION_HAPPY 0
#define EMOTION_NEUTRAL 1
#define EMOTION_SAD 2
#define EMOTION_DETERMINED 3
#define EMOTION_MAD 4
#define EMOTION_EMBARRASSED 5


#define EMOTION_TOTAL_COUNT 6


#endif