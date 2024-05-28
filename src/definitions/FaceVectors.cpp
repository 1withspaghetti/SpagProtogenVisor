#include "FaceVectors.h"


double Point::distance(Point p) {
    return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2));
}
Point& Point::moveTowards(Point p, double d) {
    double distance = this->distance(p);
    if (distance > d) {
        double angle = atan2(p.y - y, p.x - x);
        x += cos(angle) * d;
        y += sin(angle) * d;
    }
    else {
        x = p.x;
        y = p.y;
    }
    return *this;
}
Point Point::operator+(Point p) {
    return Point(x + p.x, y + p.y);
}
Point Point::operator-(Point p) {
    return Point(x - p.x, y - p.y);
}
Point Point::operator*(double d) {
    return Point(x * d, y * d);
}
Point Point::operator/(double d) {
    return Point(x / d, y / d);
}
bool Point::operator==(Point p) {
    return x == p.x && y == p.y;
}
bool Point::operator!=(Point p) {
    return x != p.x || y != p.y;
}
Point Point::copy() {
    return Point(x, y);
}

// Starting at left-most point, moving clockwise
const vector<Point> FaceVectors::eye_1 = {Point(0,6.5), Point(3,1.5), Point(5,1.5), Point(8,6.5), Point(6,6.5), Point(4,4), Point(2,6.5)}; // ^w^

const vector<Point> FaceVectors::mouth_1 = {Point(1,1), Point(6,1), Point(6,6), Point(1,6)}; // ^w^

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