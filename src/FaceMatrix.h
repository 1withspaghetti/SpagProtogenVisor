#include <Adafruit_NeoMatrix.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Faces.h>

class FaceMatrix {
    private:
        Adafruit_NeoMatrix *matrix;
        int blinkTimer;
        int brightness;
        bool talking;
        int emotion;
        int toEmotion;
        uint8_t visualizerBars[FACE_WIDTH];

        const uint16_t blush_color = matrix->Color(255, 43, 85);
        const uint16_t visualizerColors[8] = {
            Adafruit_NeoMatrix::Color(255, 0, 0), 
            Adafruit_NeoMatrix::Color(255, 78, 0), 
            Adafruit_NeoMatrix::Color(255, 156, 0), 
            Adafruit_NeoMatrix::Color(255, 234, 0), 
            Adafruit_NeoMatrix::Color(198, 255, 0), 
            Adafruit_NeoMatrix::Color(120, 255, 0), 
            Adafruit_NeoMatrix::Color(42, 255, 0), 
            Adafruit_NeoMatrix::Color(0, 255, 0)
        };
    public:
        FaceMatrix(int initialEmotion = 0, int initialBrightness = 10);
        ~FaceMatrix();
        void setup();
        void tick();
        void display(uint16_t color, float equalizerOffset);
        void setBrightness(int brightness);
        void setTalking(bool talking);
        void setEmotion(int emotion, bool force = false);
};