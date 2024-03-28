#include "OLEDDisplay.h"

#define SCREEN_WIDTH   128  // OLED display width, in pixels
#define SCREEN_HEIGHT  64   // OLED display height, in pixels
#define OLED_RESET     -1   // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

OLEDDisplay::OLEDDisplay(int initialMenu) {
    display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
    menu = initialMenu;
}

OLEDDisplay::~OLEDDisplay()
{
    delete display;
}

void OLEDDisplay::setup() {
    if(!display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    display->display();
}

void OLEDDisplay::render(int emotion, int eye_frame, int brightness, bool mic_active) {
    display->clearDisplay();
    display->clearDisplay();

    // Add text
    display->setTextColor(SSD1306_WHITE);
    display->setTextSize(2);
    display->setCursor(0, 0);
    display->print("Spag!");

    // Proto Display
    display->drawBitmap(0, 16, proto_display_bmp, PROTO_DISPLAY_WIDTH, PROTO_DISPLAY_HEIGHT, SSD1306_WHITE);
    // Current Face
    for (int y = 0; y < FACE_HEIGHT; y++) {
        uint16_t row = pgm_read_word(&((!mic_active ? face : face_talking)[getMouthIndex(emotion)][y]));
        for (int x = 0; x < FACE_WIDTH; x++) {
            if (row >> (FACE_WIDTH-1 - x) & 0b1 == 1) {
            display->fillRect(16 + x*2, 16 + 26 + y*2, 2, 2, SSD1306_WHITE);
            }
        }
    }
    // Current Eyes
    for (int y = 0; y < EYE_HEIGHT; y++) {
        uint16_t row = pgm_read_byte(&(eye[getEyeIndex(emotion)][eye_frame][y]));
        for (int x = 0; x < EYE_WIDTH; x++) {
            if (row >> (EYE_WIDTH-1 - x) & 0b1 == 1) {
            display->fillRect(13 + x*2, 16 + 15 + y*2, 2, 2, SSD1306_WHITE);
            }
        }
    }

    // Brightness Indicator
    display->drawBitmap(64 + 1, 16 + 31, brightness_bmp, BRIGHTNESS_BMP_WIDTH, BRIGHTNESS_BMP_HEIGHT, SSD1306_WHITE);
    for (int i = 0; i < brightness; i++) {
        display->drawRect(64 + 3, 16 + 27 - i*3, 9, 2, SSD1306_WHITE);
    }

    // Menu Outline
    display->drawRect(64 + 20, 16 + 1, 21, 21, SSD1306_WHITE);
    display->drawRect(64 + 42, 16 + 1, 21, 21, SSD1306_WHITE);
    display->drawRect(64 + 20, 16 + 23, 21, 21, SSD1306_WHITE);
    display->drawRect(64 + 42, 16 + 23, 21, 21, SSD1306_WHITE);

    // Menu Button
    if (menu < 3) {
        display->drawRect(64 + 46, 16 + 27, 13, 3, SSD1306_WHITE);
        display->drawRect(64 + 46, 16 + 32, 13, 3, SSD1306_WHITE);
        display->drawRect(64 + 46, 16 + 37, 13, 3, SSD1306_WHITE);
    }
    if (menu == 0) {
        drawProtoIcon(64 + 21, 16 + 2, 0);
        drawProtoIcon(64 + 43, 16 + 2, 1);
        drawProtoIcon(64 + 21, 16 + 24, 2);
    } else if (menu == 1) {
        drawProtoIcon(64 + 21, 16 + 2, 3);
        drawProtoIcon(64 + 43, 16 + 2, 4);
        drawProtoIcon(64 + 21, 16 + 24, 5);
    } else if (menu == 2){
        drawProtoIcon(64 + 21, 16 + 2, 6);
        drawProtoIcon(64 + 43, 16 + 2, 7);
        display->drawBitmap(64 + 21, 16 + 24, setting_icon_bmp, ICON_BMP_WIDTH, ICON_BMP_HEIGHT, SSD1306_WHITE);
    } else if (menu == 3) {
        display->drawBitmap(64 + 21, 16 + 2, brightness_up_icon_bmp, ICON_BMP_WIDTH, ICON_BMP_HEIGHT, SSD1306_WHITE);
        display->drawBitmap(64 + 43, 16 + 2, brightness_down_icon_bmp, ICON_BMP_WIDTH, ICON_BMP_HEIGHT, SSD1306_WHITE);
        display->drawBitmap(64 + 21, 16 + 24, reset_icon_bmp, ICON_BMP_WIDTH, ICON_BMP_HEIGHT, SSD1306_WHITE);
        display->drawBitmap(64 + 43, 16 + 24, back_icon_bmp, ICON_BMP_WIDTH, ICON_BMP_HEIGHT, SSD1306_WHITE);
    }

    display->display();
}

void OLEDDisplay::drawProtoIcon(int rel_x, int rel_y, int emotionIcon) {
    // Face
    for (int y = 0; y < FACE_HEIGHT; y++) {
        uint16_t row = pgm_read_word(&(face[getMouthIndex(emotionIcon)][y]));
        for (int x = 0; x < FACE_WIDTH; x++) {
            if (row >> (FACE_WIDTH-1 - x) & 0b1 == 1) {
                display->drawPixel(rel_x + 3 + x, rel_y + 10 + y, SSD1306_WHITE);
            }
        }
    }

    // Blush
    if (emotionIcon == 5) {
        for (int y = 0; y < FACE_HEIGHT; y++) {
            uint16_t row = pgm_read_word(&(face_blush[y]));
            for (int x = 0; x < FACE_WIDTH; x++) {
                if (row >> (FACE_WIDTH-1 - x) & 0b1 == 1) {
                display->drawPixel(rel_x + 3 + x, rel_y + 10 + y, SSD1306_WHITE);
                }
            }
        }
    }

    // Eyes
    for (int y = 0; y < EYE_HEIGHT; y++) {
        uint16_t row = pgm_read_byte(&(eye[getEyeIndex(emotionIcon)][0][y]));
        for (int x = 0; x < EYE_WIDTH; x++) {
            if (row >> (EYE_WIDTH-1 - x) & 0b1 == 1) {
                display->drawPixel(rel_x + 0 + x, rel_y + 1 + y, SSD1306_WHITE);
            }
        }
    }
}