#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "Faces.h"
#include "Icons.h"

class OLEDDisplay {
    private:
        Adafruit_SSD1306 *display;
        int menu;
        void drawProtoIcon(int rel_x, int rel_y, int emotionIcon);
    public:
        OLEDDisplay(int initialMenu = 0);
        ~OLEDDisplay();
        void setup();
        void render(int emotion, int eye_frame, int brightness, bool mic_active);
        void setMenu(int menu);
        int getMenu();
};