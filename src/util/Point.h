#include <Arduino.h>

#ifndef POINT_H
#define POINT_H

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
    const bool operator==(Point p) const;
    bool operator!=(Point p);
    Point copy();
};

#endif