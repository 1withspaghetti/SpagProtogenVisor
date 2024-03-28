#include <Arduino.h>

static float fract(float);
static float mix(float, float, float);
static float* hsv2rgb(float, float, float, float*);
