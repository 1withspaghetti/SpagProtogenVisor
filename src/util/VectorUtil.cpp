#include "VectorUtil.h"

static void interpolateVector(vector<Point>& currentVector, vector<Point>& targetVector, float amount) {
    for (int i = 0; i < currentVector.size(); i++) {
        currentVector[i] = currentVector[i] + (targetVector[i] - currentVector[i]) * amount;
    }
}