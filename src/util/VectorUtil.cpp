#include "VectorUtil.h"

/**
 * Transforms a vector of points by scaling all points away from the src point by the magnitude
 * 
 * @param original The list of points to transform
 * @param srcX The x coordinate of the source point
 * @param srcY The y coordinate of the source point
 * @param magnitude The magnitude to scale the points by, 1 is no change, 0 is all points at the src point, >1 the points are scaled away from the src point
 */
void transformVector(vector<Point>& points, double srcX, double srcY, double magnitude) {
    transformVector(points, srcX, srcY, magnitude, magnitude);
}

/**
 * Transforms a vector of points by scaling all points away from the src point by the magnitude
 * For the magnitude, 1 is no change, 0 is all points at the src point, >1 the points are scaled away from the src point
 * @param original The list of points to transform
 * @param srcX The x coordinate of the source point
 * @param srcY The y coordinate of the source point
 * @param magX Magnitude to scale the x coordinate by
 * @param magY Magnitude to scale the y coordinate by
 */
void transformVector(vector<Point>& points, double srcX, double srcY, double magX, double magY) {
    // Scale all the points away/to the src point based on the magnitude
    for (int i = 0; i < points.size(); i++) {
        points[i].scaleBy(srcX, srcY, magX, magY);
    }
}

/**
 * Translates a vector of points by a set amount in the x and y directions
 */
void translateVector(vector<Point>& points, double dx, double dy) {
    for (int i = 0; i < points.size(); i++) {
        points[i].translate(dx, dy);
    }
}

/**
 * Interpolates between two vectors of points, modifying the current vector to move towards the target vector.
 * The current vector will be resized to match the target vector if it is smaller, and points will be inserted or deleted in between if needed.
 * Each point in the current vector will be moved towards the corresponding point in the target vector by the amount a * distance + b.
 * @param current The vector of points to modify
 * @param target The vector of points to move towards, left unchanged
 * @param a The amount to move towards the target point, 0 is no movement, 1 is moving the full distance
 * @param b The amount to move towards the target point, added to the distance * a
 */
void interpolateVector(vector<Point>& current, const vector<Point>& target, double a, double b) {

    if (current.size() < target.size()) {
        
        if (current.size() == 0) {
            current.push_back(target[0].copy());
        }

        // Search points ahead to see if they are closer than the current index pair, if so, insert points in between
        // But only search far enough ahead for the in between points to fit in the difference between the current and target lengths
        for (int c = 0; c < current.size(); c++) {
            
            double minDistance = double(INFINITY);
            int minIndexT = 0;

            for (int t = c; t < min(c + target.size() - current.size(), target.size() - 1); t++) {
                double distance = current[c].distance(target[t]);
                if (distance < minDistance) {
                    minDistance = distance;
                    minIndexT = t;
                }
            }

            if (minIndexT != c) {
                Point toCopy = target[minIndexT];
                for (int i = 0; i < minIndexT - c; i++) {
                    current.insert(current.begin() + c, toCopy.copy());
                }
            }

            if (current.size() == target.size()) break;
        }

        // Handle the case where the current vector is still smaller than the target vector
        for (int i = 0; i < target.size() - current.size(); i++) {
            current.push_back(current[current.size() - 1].copy());
        }
    } else if (current.size() > target.size()) {
        
        // Search points ahead to see if they are closer than the current index pair, if so, remove points in between
        for (int c = 0; c < current.size(); c++) {
            
            double minDistance = double(INFINITY);
            int minIndexT = -1;

            int min1 = c + current.size() - target.size();
            int min2 = target.size() - 1;
            for (int t = c; t < min(min1, min2); t++) {
                double distance = current[c].distance(target[t]);
                if (distance < minDistance) {
                    minDistance = distance;
                    minIndexT = t;
                }
            }

            if (minIndexT > 0 && minIndexT != c) {
                //Serial.println("Removing at "+ String(c) + " to " + String(minIndexT));
                current.erase(std::next(current.begin(), c), std::next(current.begin(), minIndexT));
            }

            if (current.size() == target.size()) break;
        }

        // Handle the case where the current vector is still larger than the target vector
        int toRemove = current.size() - target.size();
        for (int i = 0; i < toRemove; i++) {
            current.pop_back();
        }
    }

    // Interpolate the points
    for (int i = 0; i < current.size(); i++) {
        current[i].moveTowards(target[i], a, b);
    }
}
 /**
  * Checks if a point is inside a polygon defined by a vector of points
  */
bool pointInPolygon(const vector<Point>& vector, Point p) {
    bool inside = false;
    Point a = vector[0];
    Point b;
    for (int i = 1; i <= vector.size(); i++) {
        b = vector[i % vector.size()];

        if (p.y > min(a.y, b.y) && p.y <= max(a.y, b.y) && p.x <= max(a.x, b.x) && a.y != b.y) {
            double xIntersection = (p.y - a.y) * (b.x - a.x) / (b.y - a.y) + a.x;
            if (a.x == b.x || p.x <= xIntersection) {
                inside = !inside;
            }
        }

        a = b;
    }
    return inside;
}