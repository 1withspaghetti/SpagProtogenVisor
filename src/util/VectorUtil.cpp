#include "VectorUtil.h"

void interpolateVector(vector<Point>& currentVector, vector<Point>& targetVector, double amount) {

    // If the current vector is larger than the target vector, remove points from the current vector
    if (currentVector.size() > targetVector.size()) {
        // Pairs each of the target points with the closest current point
        bool *const taken = new bool[currentVector.size()];
        for (int i = 0; i < targetVector.size(); i++) {
            double closestDistance = double(INFINITY);
            int closestIndex = 0;
            for (int j = 0; j < currentVector.size(); j++) {
                if (!taken[j]) {
                    double distance = currentVector[j].distance(targetVector[i]);
                    if (distance < closestDistance) {
                        closestDistance = distance;
                        closestIndex = j;
                    }
                }
            }
            taken[closestIndex] = true;
        }
        // Delete points that do not have a pair
        int offset = 0;
        for (int i = 0; i < currentVector.size(); i++) {
            if (!taken[i]) {
                currentVector.erase(currentVector.begin() + i - offset);
                offset++; // Increment offset to account for the removed point when indexing
            }
        }
        delete[] taken;
    }

    // If the current vector is smaller than the target vector, add points to the current vector
    else
    if (currentVector.size() < targetVector.size()) {
        // Pairs each of the current points with the closest target point
        bool *const taken = new bool[targetVector.size()];
        for (int i = 0; i < currentVector.size(); i++) {
            double closestDistance;
            int closestIndex;
            for (int j = 0; j < targetVector.size(); j++) {
                if (!taken[j]) {
                    double distance = currentVector[i].distance(targetVector[j]);
                    if (distance < closestDistance) {
                        closestDistance = distance;
                        closestIndex = j;
                    }
                }
            }
            taken[closestIndex] = true;
        }
        // For each target point that does not have a pair, clone the closest current point
        int offset = 0;
        for (int i = 0; i < targetVector.size(); i++) {
            if (!taken[i]) {
                double closestDistance;
                int closestIndex;
                for (int j = 0; j < currentVector.size(); j++) {
                    double distance = currentVector[j].distance(targetVector[i]);
                    if (distance < closestDistance) {
                        closestDistance = distance;
                        closestIndex = j;
                    }
                }
                currentVector.insert(currentVector.begin() + i + offset, currentVector[closestIndex]);
                offset++; // Increment offset to account for the new point when indexing
            }
        }
        delete[] taken;
    }
    // Once the current vector is the same size as the target vector, interpolate each point

    for (int i = 0; i < currentVector.size(); i++) {
        currentVector[i] = currentVector[i] + (targetVector[i] - currentVector[i]) * amount;
    }
}

bool pointInPolygon(vector<Point>& vector, Point p) {
    bool inside = false;
    Point a = vector[0];
    for (int i = 1; i <= vector.size(); i++) {
        Point b = vector[i % vector.size()];

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