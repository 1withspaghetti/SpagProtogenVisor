#include "FaceVectors.h"

// ==============================================================================
//                               EYE VECTORS
// ==============================================================================
// Starting at left-most point, moving clockwise

const vector<Point> FaceVectors::eye_1 = {
    Point( 1, 6 ), 
    Point( 3, 2 ), 
    Point( 5, 2 ), 
    Point( 7, 6 ), 
    Point( 5.5, 6 ), 
    Point( 4, 3 ), 
    Point( 2.5, 6 )
}; // ^w^
const vector<Point> FaceVectors::eye_2 = {
    Point( 0.5, 5.5 ), 
    Point( 2, 3 ), 
    Point( 4, 2 ), 
    Point( 6, 3 ), 
    Point( 7.5, 5.5 ), 
    Point( 5, 4.5 ), 
    Point( 3, 4.5 )
}; // ^w^ but less
const vector<Point> FaceVectors::eye_3 = {
    Point( 1, 4 ), 
    Point( 2, 2.5 ), 
    Point( 4, 2 ), 
    Point( 6, 3 ), 
    Point( 7, 4 ), 
    Point( 5, 5.5 ), 
    Point( 2, 5.5 )
}; // almond
const vector<Point> FaceVectors::eye_4 = {
    Point( 1.0, 4.0 ), 
    Point( 1.5, 2.5 ), 
    Point( 3.5, 2.5 ), 
    Point( 5.5, 3.5 ), 
    Point( 7.0, 5.0 ), 
    Point( 5.0, 6.5 ), 
    Point( 2.0, 5.5 )
}; // mad almond
const vector<Point> FaceVectors::eye_5 = {
    Point( 1.5, 4.0 ), 
    Point( 2.5, 2.5 ), 
    Point( 4.0, 1.5 ), 
    Point( 5.5, 2.5 ), 
    Point( 6.5, 4.0 ), 
    Point( 5.5, 5.5 ), 
    Point( 4.0, 6.5 ),
    Point( 2.5, 5.5 )
}; // OwO


// ==============================================================================
//                               MOUTH VECTORS
// ==============================================================================
// Starting at left-most point, moving clockwise

const vector<Point> FaceVectors::mouth_1 = {
    Point( 03.0, 4.5 ), 
    Point( 05.0, 6.5 ), 
    Point( 07.0, 7.0 ), 
    Point( 10.0, 6.0 ), 
    Point( 13.0, 7.0 ), 
    Point( 16.0, 6.0 ), 
    Point( 16.0, 7.0 ),
    Point( 13.0, 8.0 ),
    Point( 10.0, 7.0 ),
    Point( 07.0, 8.0 ),
    Point( 05.0, 7.5 ),
    Point( 03.5, 6.5 ),
}; // ^w^

const vector<Point> FaceVectors::mouth_2 = {
    Point( 03.0, 6.0 ), 
    Point( 05.0, 6.5 ), 
    Point( 07.0, 7.0 ), 
    Point( 10.0, 6.0 ), 
    Point( 13.0, 7.0 ), 
    Point( 16.0, 6.0 ), 
    Point( 16.0, 7.0 ),
    Point( 13.0, 8.0 ),
    Point( 10.0, 7.0 ),
    Point( 07.0, 8.0 ),
    Point( 05.0, 7.5 ),
    Point( 03.5, 7.0 ),
}; // ^-^

const vector<Point> FaceVectors::mouth_3 = {
    Point( 03.0, 8.0 ), 
    Point( 04.0, 6.5 ), 
    Point( 07.0, 6.0 ), 
    Point( 10.0, 6.0 ), 
    Point( 13.0, 7.0 ), 
    Point( 16.0, 6.0 ), 
    Point( 16.0, 7.0 ),
    Point( 13.0, 8.0 ),
    Point( 10.0, 7.0 ),
    Point( 07.0, 7.0 ),
    Point( 05.5, 7.5 ),
    Point( 04.0, 7.75 ),
}; // ^m^

const vector<Point> FaceVectors::special = {
    Point( 6.5, 6.0 ), 
    Point( 9.0, 6.0 ), 
    Point( 7.0, 8.0 ), 
}; // vampire fang

const vector<Point> FaceVectors::blank = {};


// ==============================================================================
//                            EMOTION DEFINITIONS
// ==============================================================================

const vector<Point>& FaceVectors::getEyeVector(int emotion) {
    switch (emotion) {
        case EMOTION_HAPPY:
            return eye_1;
        case EMOTION_NEUTRAL:
            return eye_2;
        case EMOTION_SAD:
            return eye_3;
        case EMOTION_DETERMINED:
            return eye_4;
        case EMOTION_MAD:
            return eye_4;
        case EMOTION_EMBARRASSED:
            return eye_5;
        
        default:
            return eye_2;
    }
}

const vector<Point>& FaceVectors::getMouthVector(int emotion) {
    switch (emotion) {
        case EMOTION_HAPPY:
            return mouth_1;
        case EMOTION_NEUTRAL:
            return mouth_2;
        case EMOTION_SAD:
            return mouth_3;
        case EMOTION_DETERMINED:
            return mouth_1;
        case EMOTION_MAD:
            return mouth_3;
        case EMOTION_EMBARRASSED:
            return mouth_3;
        
        default:
            return mouth_2;
    }
}

const vector<Point>& FaceVectors::getSpecialVector(int emotion) {
    switch (emotion) {
        case EMOTION_HAPPY:
        case EMOTION_NEUTRAL:
        case EMOTION_DETERMINED:
            return special;
        default:
            return blank;
    }
}