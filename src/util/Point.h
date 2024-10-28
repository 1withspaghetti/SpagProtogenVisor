#include <Arduino.h>

#ifndef POINT_H
#define POINT_H

struct Point {
    double x, y;

    /**
     * Default constructor for a point, sets the x and y values to 0
     */
    Point();
    /**
     * Constructor for a point, sets the x and y values to the given values
     */
    Point(double x, double y);
    /**
     * Copy constructor for a point, sets the x and y values to the given point
     */
    Point(const Point& p);
    /**
     * Destructor for a point
     */
    ~Point();
    /**
     * Calculates the distance between this point and another point
     * @param p The point to calculate the distance to
     * @return The distance between the two points
     */
    double distance(Point p);
    /**
     * Moves this point towards another point by a set distance, if the distance to the point is less than the distance to move, the point will be moved to the target point
     * @param p The point to move towards
     * @param d The max distance to move
     * @return A reference to this point after moving
     */
    Point& moveTowards(Point p, double d);
    /**
     * Moves this point towards another point depending on a the function [toMove = distance * a + b]
     * @param p The point to move towards
     * @param a The multiplier for the distance
     * @param b The offset for the distance
     * @return A reference to this point after moving
     */
    Point& moveTowards(Point p, double a, double b);
    /**
     * Scales the distance between this point and another point by a set magnitude
     */
    Point& scaleBy(double srcX, double srcY, double magX, double magY);
    /**
     * Translates this point by a set amount in the x and y directions
     */
    Point& translate(double dx, double dy);
    /**
     * Adds two points together, returning a new point with the sum of the x and y values
     */
    Point operator+(Point p);
    /**
     * Subtracts two points, returning a new point with the difference of the x and y values
     */
    Point operator-(Point p);
    /**
     * Multiplies a point by a scalar, returning a new point with the x and y values multiplied by the scalar
     */
    Point operator*(double d);
    /**
     * Divides a point by a scalar, returning a new point with the x and y values divided by the scalar
     */
    Point operator/(double d);
    /**
     * Compares two points, returning true if the x and y values are equal
     */
    bool operator==(Point p);
    /**
     * Compares two points, returning true if the x and y values are equal
     */
    const bool operator==(Point p) const;
    /**
     * Compares two points, returning true if the x and y values are not equal
     */
    const bool operator!=(Point p) const;
    /**
     * Creates a copy of this point with the same x and y values
     */
    const Point copy() const;
};

#endif