#include "Point.h"

Point::Point() : x(0), y(0) {}
Point::Point(double x, double y) : x(x), y(y) {}
Point::Point(const Point& p) : x(p.x), y(p.y) {}
Point::~Point() {}
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
Point& Point::moveTowards(Point p, double a, double b) {
    double distance = this->distance(p);
    double amountToMove = distance * a + b;
    return this->moveTowards(p, amountToMove);
}
Point& Point::scaleBy(double srcX, double srcY, double magX, double magY) {
    double x = this->x - srcX;
    double y = this->y - srcY;
    double angle = atan2(y, x);
    double distance = sqrt(x * x + y * y);
    this->x = srcX + cos(angle) * (distance * magX);
    this->y = srcY + sin(angle) * (distance * magY);
    return *this;
}
Point& Point::translate(double dx, double dy) {
    x += dx;
    y += dy;
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
const bool Point::operator==(Point p) const {
    return x == p.x && y == p.y;
}
const bool Point::operator!=(Point p) const {
    return x != p.x || y != p.y;
}
const Point Point::copy() const {
    return Point(x, y);
}