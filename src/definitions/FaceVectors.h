#include <Arduino.h>
#include <vector>
#include "../util/Point.h"
using namespace std;

#ifndef FACE_VECTORS_H
#define FACE_VECTORS_H

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

#endif