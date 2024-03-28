#include "FaceMatrix.h"

// Neo Matrix Constants
#define NEO_MATRIX_DATA_PIN 3
#define NEO_MATRIX_WIDTH 48
#define NEO_MATRIX_HEIGHT 8

#define NEO_MATRIX_BRIGHTNESS_MULTIPLIER 3 // Brightness can be changed from 1-10 times this number


FaceMatrix::FaceMatrix(int initialEmotion, int initialBrightness) {
    matrix = new Adafruit_NeoMatrix(NEO_MATRIX_WIDTH, NEO_MATRIX_HEIGHT, NEO_MATRIX_DATA_PIN,
        NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
        NEO_GRB + NEO_KHZ800);

    emotion = initialEmotion;
    toEmotion = -1;
    brightness = initialBrightness;
    talking = false;
    blinkTimer = 100;
}

FaceMatrix::~FaceMatrix()
{
    delete matrix;
}

void FaceMatrix::setup() {
    matrix->begin();
    matrix->setTextWrap(false);
    matrix->setBrightness(brightness * NEO_MATRIX_BRIGHTNESS_MULTIPLIER);
}

void FaceMatrix::tick() {
    // Blinks
    int eye_frame = EYE_FRAMES-1-min(abs(blinkTimer), EYE_FRAMES-1);
    if (eye_frame == 2 && toEmotion != -1) {
        emotion = toEmotion;
        toEmotion = -1;
    }
    //if (abs(blinkTimer) == EYE_FRAMES-1 || blinkTimer == 0) hasChange = true;
    
    blinkTimer--;
    if (blinkTimer <= -EYE_FRAMES) blinkTimer = random(100,150);
}

void FaceMatrix::display(uint16_t color, float equalizerOffset) {

    matrix->fillScreen(0);

    // Audio Visualizer
    if (emotion == 7) {
        for (int i = 0; i < FACE_WIDTH; i++) {
            uint8_t newVal = visualizerBars[i] + (random(3)-1);
            visualizerBars[i] = constrain(newVal, 1, 4);
            for (int y = 0; y < FACE_HEIGHT; y++) {
                if (y >= FACE_HEIGHT - visualizerBars[i] - equalizerOffset) matrix->fillRect(EYE_WIDTH + i*2, y, 2, 1, visualizerColors[y]);
            }
        }
    }
    else if (emotion == 5) {} // No face on blush
    else {
        // Face
        for (int y = 0; y < FACE_HEIGHT; y++) {
            uint16_t row = pgm_read_word(&((!talking ? face : face_talking)[getMouthIndex(emotion)][y]));
            for (int x = 0; x < FACE_WIDTH; x++) {
                if (row >> (FACE_WIDTH-1 - x) & 0b1 == 1) {
                    matrix->drawPixel(EYE_WIDTH + x, y, color);
                    matrix->drawPixel((EYE_WIDTH + (FACE_WIDTH * 2)) - 1 - x, y, color);
                }
            }
        }
    }
    // If blush should be applied
    if (emotion == 5) {
        for (int y = 0; y < FACE_HEIGHT; y++) {
            uint16_t row = pgm_read_word(&(face_blush[y]));
            for (int x = 0; x < FACE_WIDTH; x++) {
                if (row >> (FACE_WIDTH-1 - x) & 0b1 == 1) {
                    matrix->drawPixel(EYE_WIDTH + x, y, blush_color);
                    matrix->drawPixel((EYE_WIDTH + (FACE_WIDTH * 2)) - 1 - x, y, blush_color);
                }
            }
        }
    }

    // Eyes
    int eye_frame = EYE_FRAMES-1-min(abs(blinkTimer), EYE_FRAMES-1);

    for (int y = 0; y < EYE_HEIGHT; y++) {
        uint8_t row = pgm_read_byte(&(eye[getEyeIndex(emotion)][eye_frame][y]));
        for (int x = 0; x < EYE_WIDTH; x++) {
            if (row >> (EYE_WIDTH-1 - x) & 0b1 == 1) {
                matrix->drawPixel(x, y, color);
                matrix->drawPixel(NEO_MATRIX_WIDTH-1-x, y, color);
            }
        }
    }

    matrix->show();
}

void FaceMatrix::setEmotion(int newEmotion, bool force) {
    if (force) {
        emotion = newEmotion;
    } else {
        toEmotion = newEmotion;
        blinkTimer = EYE_FRAMES-1;
    }
}

void FaceMatrix::setBrightness(int newBrightness) {
    brightness = newBrightness;
    matrix->setBrightness(brightness * NEO_MATRIX_BRIGHTNESS_MULTIPLIER);
}

void FaceMatrix::setTalking(bool newTalking) {
    talking = newTalking;
}

int FaceMatrix::getEyeFrame() {
    return EYE_FRAMES-1-min(abs(blinkTimer), EYE_FRAMES-1);
}