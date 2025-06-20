#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "settings.h"
#include "definitions/Icons.h"
#include "util/VectorUtil.h"
#include "definitions/FaceVectors.h"
#include "components/BLEServer.h"


class OLEDDisplay {
    private:
        Adafruit_SSD1306 *display;
        bool initialized = false;

        bool needsRender = true;
        int menu = 0;
        
        void drawProtoIcon(int rel_x, int rel_y, int emotionIcon);
        uint8_t preRenderedIcons[EMOTION_TOTAL_COUNT][(int)ceil(ICON_BMP_WIDTH * ICON_BMP_HEIGHT / 8.0)]; // Each bit represents a pixel
    public:
        OLEDDisplay();
        ~OLEDDisplay();
        void setup();
        void render(int brightness, vector<Point>& eyeVector, vector<Point>& mouthVector, ProtoBLEStatus bleStatus);
        void setNeedsRender(bool needsRender);
        void setMenu(int menu);
        int getMenu();
};