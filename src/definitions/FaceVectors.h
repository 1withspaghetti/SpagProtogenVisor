#include <Arduino.h>
#include <vector>
using namespace std;

struct Point {
    double x, y;
    Point(double x, double y) : x(x), y(y) {}
    double distance(Point p) {
        return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2));
    }
    Point operator+(Point p) {
        return Point(x + p.x, y + p.y);
    }
    Point operator-(Point p) {
        return Point(x - p.x, y - p.y);
    }
    Point operator*(double d) {
        return Point(x * d, y * d);
    }
    Point operator/(double d) {
        return Point(x / d, y / d);
    }
};

#define P Point


class FaceVectors {
    public:
        static const vector<Point>& getEyeVector(int emotion);
        static const vector<Point>& getMouthVector(int emotion);

    private:
        static const vector<Point> eye_1;
        static const vector<Point> mouth_1;
};

const vector<Point> FaceVectors::eye_1 = {P(1,1), P(6,1), P(6,6), P(1,6)}; // ^w^

const vector<Point> FaceVectors::mouth_1 = {P(1,1), P(6,1), P(6,6), P(1,6)}; // ^w^

const vector<Point>& FaceVectors::getEyeVector(int emotion) {
    switch (emotion) {
        case 0:
            return eye_1;
        default:
            return eye_1;
    }
}

const vector<Point>& FaceVectors::getMouthVector(int emotion) {
    switch (emotion) {
        case 0:
            return mouth_1;
        default:
            return mouth_1;
    }
}