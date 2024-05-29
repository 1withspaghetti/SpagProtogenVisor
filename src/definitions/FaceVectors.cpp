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
const vector<Point> FaceVectors::eye_1 = {
    Point( 0, 6.5 ), 
    Point( 3, 1.5 ), 
    Point( 5, 1.5 ), 
    Point( 8, 6.5 ), 
    Point( 6, 6.5 ), 
    Point( 4, 4 ), 
    Point( 2, 6.5 )
}; // ^w^
const vector<Point> FaceVectors::eye_2 = {
    Point( 0, 6 ), 
    Point( 1, 3 ), 
    Point( 4, 2 ), 
    Point( 7, 3 ), 
    Point( 8, 6 ), 
    Point( 5, 5 ), 
    Point( 3, 5 )
}; // ^w^ but less
const vector<Point> FaceVectors::eye_3 = {
    Point( 0, 4 ), 
    Point( 1.5, 2.5 ), 
    Point( 4, 2 ), 
    Point( 7, 3 ), 
    Point( 8, 5 ), 
    Point( 5, 6 ), 
    Point( 2, 6 )
}; // almond
const vector<Point> FaceVectors::eye_4 = {
    Point( 0.0, 2.0 ), 
    Point( 3.5, 2.5 ), 
    Point( 6.5, 3.5 ), 
    Point( 8.0, 6.0 ), 
    Point( 5.0, 6.5 ), 
    Point( 2.0, 6.0 ), 
    Point( 0.5, 4.5 )
}; // almond
const vector<Point> FaceVectors::eye_5 = {
    Point( 1.0, 4.0 ), 
    Point( 2.0, 2.0 ), 
    Point( 4.0, 1.0 ), 
    Point( 6.0, 2.0 ), 
    Point( 7.0, 4.0 ), 
    Point( 6.0, 6.0 ), 
    Point( 4.0, 7.0 ),
    Point( 2.0, 6.0 )
}; // OwO

const vector<Point> FaceVectors::mouth_1 = {Point(1,1), Point(6,1), Point(6,6), Point(1,6)}; // ^w^

const vector<Point>& FaceVectors::getEyeVector(int emotion) {
    switch (emotion) {
        case 0:
            return eye_1;
        case 1:
            return eye_2;
        case 2:
            return eye_3;
        case 3:
            return eye_4;
        case 4:
            return eye_5;
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