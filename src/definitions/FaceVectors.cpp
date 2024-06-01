#include "FaceVectors.h"

// ==============================================================================
//                               EYE VECTORS
// ==============================================================================
// Starting at left-most point, moving clockwise

const vector<Point> FaceVectors::eye_1 = {
    Point( 0, 6.5 ), 
    Point( 3, 1.5 ), 
    Point( 5, 1.5 ), 
    Point( 8, 6.5 ), 
    Point( 6, 6.5 ), 
    Point( 4, 4 ), 
    Point( 2, 6.5 )
}; // ^w^
const vector<Point> FaceVectors::eye_2 = {
    Point( 0, 6 ), 
    Point( 1, 3 ), 
    Point( 4, 2 ), 
    Point( 7, 3 ), 
    Point( 8, 6 ), 
    Point( 5, 5 ), 
    Point( 3, 5 )
}; // ^w^ but less
const vector<Point> FaceVectors::eye_3 = {
    Point( 0, 4 ), 
    Point( 1.5, 2.5 ), 
    Point( 4, 2 ), 
    Point( 7, 3 ), 
    Point( 8, 5 ), 
    Point( 5, 6 ), 
    Point( 2, 6 )
}; // almond
const vector<Point> FaceVectors::eye_4 = {
    Point( 0.0, 2.0 ), 
    Point( 3.5, 2.5 ), 
    Point( 6.5, 3.5 ), 
    Point( 8.0, 6.0 ), 
    Point( 5.0, 6.5 ), 
    Point( 2.0, 6.0 ), 
    Point( 0.5, 4.5 )
}; // mad almond
const vector<Point> FaceVectors::eye_5 = {
    Point( 1.0, 4.0 ), 
    Point( 2.0, 2.0 ), 
    Point( 4.0, 1.0 ), 
    Point( 6.0, 2.0 ), 
    Point( 7.0, 4.0 ), 
    Point( 6.0, 6.0 ), 
    Point( 4.0, 7.0 ),
    Point( 2.0, 6.0 )
}; // OwO


// ==============================================================================
//                               MOUTH VECTORS
// ==============================================================================
// Starting at left-most point, moving clockwise

const vector<Point> FaceVectors::mouth_1 = {
    Point( 03.0, 4.5 ), 
    Point( 05.0, 6.0 ), 
    Point( 07.0, 6.5 ), 
    Point( 10.0, 6.0 ), 
    Point( 13.0, 6.5 ), 
    Point( 16.0, 6.0 ), 
    Point( 16.0, 7.5 ),
    Point( 13.0, 8.0 ),
    Point( 10.0, 7.5 ),
    Point( 07.0, 8.0 ),
    Point( 05.0, 7.5 ),
    Point( 03.5, 6.5 ),
}; // ^w^

const vector<Point> FaceVectors::mouth_2 = {
    Point( 03.0, 6.0 ), 
    Point( 05.0, 6.5 ), 
    Point( 07.0, 6.5 ), 
    Point( 10.0, 6.0 ), 
    Point( 13.0, 6.5 ), 
    Point( 16.0, 6.0 ), 
    Point( 16.0, 7.5 ),
    Point( 13.0, 8.0 ),
    Point( 10.0, 7.5 ),
    Point( 07.0, 8.0 ),
    Point( 05.0, 7.5 ),
    Point( 03.5, 7.0 ),
}; // ^-^

const vector<Point> FaceVectors::mouth_3 = {
    Point( 03.0, 8.0 ), 
    Point( 04.0, 6.5 ), 
    Point( 07.0, 6.0 ), 
    Point( 10.0, 6.0 ), 
    Point( 13.0, 6.5 ), 
    Point( 16.0, 6.0 ), 
    Point( 16.0, 7.5 ),
    Point( 13.0, 8.0 ),
    Point( 10.0, 7.5 ),
    Point( 07.0, 7.25 ),
    Point( 05.5, 7.5 ),
    Point( 04.0, 7.75 ),
}; // ^m^


// ==============================================================================
//                            EMOTION DEFINITIONS
// ==============================================================================

#define EMOTION_HAPPY 0
#define EMOTION_NEUTRAL 1
#define EMOTION_SAD 2
#define EMOTION_DETERMINED 3
#define EMOTION_MAD 4
#define EMOTION_EMBARRASSED 5


#define EMOTION_TOTAL_COUNT 6

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