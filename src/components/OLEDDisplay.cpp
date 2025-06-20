#include "OLEDDisplay.h"

#define SCREEN_WIDTH   128  // OLED display width, in pixels
#define SCREEN_HEIGHT  64   // OLED display height, in pixels
#define OLED_RESET     -1   // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

OLEDDisplay::OLEDDisplay() {
    display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
}

OLEDDisplay::~OLEDDisplay()
{
    delete display;
}

void OLEDDisplay::setup() {
    #ifdef VERBOSE
    Serial.println("OLED: Pre-rendering emotion icons...");
    #endif
    for (uint8_t i = 0; i < EMOTION_TOTAL_COUNT; i++) {

        for (uint8_t y = 0; y < EYE_HEIGHT; y++) {
            for (uint8_t x = 0; x < EYE_WIDTH; x++) {
                if (pointInPolygon(FaceVectors::getEyeVector(i), {x+0.5, y+0.5})) {
                    uint8_t ix = x + 1;
                    uint8_t iy = y + 1;
                    uint16_t pixelIndex = (iy * ICON_BMP_WIDTH + ix);
                    uint8_t byte = preRenderedIcons[i][pixelIndex / 8];
                    preRenderedIcons[i][pixelIndex / 8] = byte | (1 << (pixelIndex % 8));
                }
            }
        }

        for (uint8_t y = 0; y < FACE_HEIGHT; y++) {
            for (uint8_t x = 0; x < FACE_WIDTH; x++) {
                if (pointInPolygon(FaceVectors::getMouthVector(i), {x+0.5, y+0.5})) {
                    uint8_t ix = x + 3;
                    uint8_t iy = y + 9;
                    uint16_t pixelIndex = (iy * ICON_BMP_WIDTH + ix);
                    uint8_t byte = preRenderedIcons[i][pixelIndex / 8];
                    preRenderedIcons[i][pixelIndex / 8] = byte | (1 << (pixelIndex % 8));
                }
            }
        }
    }
    #ifdef VERBOSE
    Serial.println("OLED: Finished pre-rendering icons.");
    #endif


    if(display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        initialized = true;
    } else {
        Serial.println("ERROR: SSD1306 allocation failed");
    }
    display->display();
    delay(100);
    display->clearDisplay();
}

void OLEDDisplay::render(int brightness, vector<Point>& eyeVector, vector<Point>& mouthVector, ProtoBLEStatus bleStatus) {
    if (!needsRender) return;
    if (!initialized) {
        #ifdef VERBOSE
        Serial.println("ERROR: OLED Display not initialized");
        #endif
        return;
    }
    display->clearDisplay();

    // Add text
    display->setTextColor(SSD1306_WHITE);
    display->setTextSize(2);
    display->setCursor(0, 0);
    display->print("Spag!");

    // Bluetooth connected icons, right above the menu buttons
    display->drawBitmap(64 + 20 + 2, 0, bleStatus.leftConnected ? paw_connected_icon : paw_disconnected_icon, PAW_ICON_WIDTH, PAW_ICON_HEIGHT, SSD1306_WHITE);
    display->drawBitmap(64 + 42 + 2, 0, bleStatus.rightConnected ? paw_connected_icon : paw_disconnected_icon, PAW_ICON_WIDTH, PAW_ICON_HEIGHT, SSD1306_WHITE);
    // Pixel locations of beans on icon: (1,4) (7, 2) (13, 4)
    if (bleStatus.leftValue >> 2 & 0b1) display->drawRect(64 + 20 + 2 + 1, 4, 2, 2, SSD1306_WHITE);
    if (bleStatus.leftValue >> 1 & 0b1) display->drawRect(64 + 20 + 2 + 7, 2, 2, 2, SSD1306_WHITE);
    if (bleStatus.leftValue >> 0 & 0b1) display->drawRect(64 + 20 + 2 + 13, 4, 2, 2, SSD1306_WHITE);
    if (bleStatus.rightValue >> 0 & 0b1) display->drawRect(64 + 42 + 2 + 1, 4, 2, 2, SSD1306_WHITE);
    if (bleStatus.rightValue >> 1 & 0b1) display->drawRect(64 + 42 + 2 + 7, 2, 2, 2, SSD1306_WHITE);
    if (bleStatus.rightValue >> 2 & 0b1) display->drawRect(64 + 42 + 2 + 13, 4, 2, 2, SSD1306_WHITE);

    // Proto Display
    display->drawBitmap(0, 16, proto_display_bmp, PROTO_DISPLAY_WIDTH, PROTO_DISPLAY_HEIGHT, SSD1306_WHITE);
    // Current Face
    
    // Draw the eyes
    #define EYE_SCALING 1.75
    for (int y = 0; y < EYE_HEIGHT * EYE_SCALING; y++) {
        for (int x = 0; x < EYE_WIDTH * EYE_SCALING; x++) {
            if (pointInPolygon(eyeVector, {x/EYE_SCALING, y/EYE_SCALING})) {
                display->drawPixel(15 + x, 16 + 16 + y, SSD1306_WHITE);
            }
        }
    }
    // Draw the face
    #define FACE_SCALING 1.75
    for (int y = 0; y < FACE_HEIGHT * FACE_SCALING; y++) {
        for (int x = 0; x < FACE_WIDTH * FACE_SCALING; x++) {
            if (pointInPolygon(mouthVector, {x/FACE_SCALING, y/FACE_SCALING})) {
                display->drawPixel(16 + x, 16 + 26 + y, SSD1306_WHITE);
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
        //drawProtoIcon(64 + 21, 16 + 2, 6);
        //drawProtoIcon(64 + 43, 16 + 2, 7);
        display->drawBitmap(64 + 21, 16 + 24, setting_icon_bmp, ICON_BMP_WIDTH, ICON_BMP_HEIGHT, SSD1306_WHITE);
    } else if (menu == 3) {
        display->drawBitmap(64 + 21, 16 + 2, brightness_down_icon_bmp, ICON_BMP_WIDTH, ICON_BMP_HEIGHT, SSD1306_WHITE);
        display->drawBitmap(64 + 43, 16 + 2, brightness_up_icon_bmp, ICON_BMP_WIDTH, ICON_BMP_HEIGHT, SSD1306_WHITE);
        display->drawBitmap(64 + 21, 16 + 24, reset_icon_bmp, ICON_BMP_WIDTH, ICON_BMP_HEIGHT, SSD1306_WHITE);
        display->drawBitmap(64 + 43, 16 + 24, back_icon_bmp, ICON_BMP_WIDTH, ICON_BMP_HEIGHT, SSD1306_WHITE);
    }

    display->display();
    needsRender = false;
}

void OLEDDisplay::drawProtoIcon(int rel_x, int rel_y, int emotionIcon) {
    //display->drawBitmap(rel_x, rel_y, preRenderedIcons[emotionIcon], ICON_BMP_WIDTH, ICON_BMP_HEIGHT, SSD1306_WHITE);
    for (int y = 0; y < ICON_BMP_HEIGHT; y++) {
        for (int x = 0; x < ICON_BMP_WIDTH; x++) {
            uint16_t pixelIndex = y * ICON_BMP_WIDTH + x;
            if (preRenderedIcons[emotionIcon][pixelIndex / 8] & (1 << (pixelIndex % 8))) {
                display->writePixel(rel_x + x, rel_y + y, SSD1306_WHITE);
            }
        }
    }
}

void OLEDDisplay::setNeedsRender(bool needsRender) {
    this->needsRender = needsRender;
}

/**
 * Set the current menu, also sets the needsRender flag to true
 */
void OLEDDisplay::setMenu(int menu) {
    this->menu = menu;
    needsRender = true;
}

int OLEDDisplay::getMenu() {
    return menu;
}
