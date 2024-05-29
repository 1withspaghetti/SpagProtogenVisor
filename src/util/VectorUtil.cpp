#include "VectorUtil.h"

void interpolateVector(vector<Point>& current, vector<Point>& target, double amount) {

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
                Serial.println("Removing at "+ String(c) + " to " + String(minIndexT));
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
        current[i].moveTowards(target[i], amount);
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