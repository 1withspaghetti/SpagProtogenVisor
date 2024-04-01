#include "definitions/FaceVectors.h"

static void interpolateVector(vector<Point>& currentVector, vector<Point>& targetVector, double amount);

static bool pointInPolygon(vector<Point>& vector, Point p);