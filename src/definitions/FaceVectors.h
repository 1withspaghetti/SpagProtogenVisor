#include <Arduino.h>
#include <vector>
using namespace std;

#ifndef FACE_VECTORS_H
#define FACE_VECTORS_H

struct Point {
    double x, y;
    Point(double x, double y) : x(x), y(y) {}
    double distance(Point p);
    Point& moveTowards(Point p, double d);
    Point operator+(Point p);
    Point operator-(Point p);
    Point operator*(double d);
    Point operator/(double d);
    bool operator==(Point p);
    bool operator!=(Point p);
    Point copy();
};

class FaceVectors {
    public:
        static const vector<Point>& getEyeVector(int emotion);
        static const vector<Point>& getMouthVector(int emotion);

    private:
        static const vector<Point> eye_1;
        static const vector<Point> mouth_1;
};

#endif