#include "util/Point.h"
#include <vector>
using namespace std;

void transformVector(const vector<Point>& original, vector<Point>& result, double srcX, double srcY, double magnitude);

void transformVector(const vector<Point>& original, vector<Point>& result, double srcX, double srcY, double magX, double magY);

void interpolateVector(vector<Point>& currentVector, const vector<Point>& targetVector, double a, double b);

bool pointInPolygon(const vector<Point>& vector, Point p);