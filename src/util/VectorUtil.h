#include "util/Point.h"
#include <vector>
using namespace std;

void transformVector(vector<Point>& points, double srcX, double srcY, double magnitude);

void transformVector(vector<Point>& points, double srcX, double srcY, double magX, double magY);

void interpolateVector(vector<Point>& currentVector, const vector<Point>& targetVector, double a, double b);

bool pointInPolygon(const vector<Point>& vector, Point p);