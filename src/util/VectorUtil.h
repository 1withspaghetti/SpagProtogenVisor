#include "util/Point.h"
#include <vector>
using namespace std;

void interpolateVector(vector<Point>& currentVector, vector<Point>& targetVector, double amount);

bool pointInPolygon(const vector<Point>& vector, Point p);