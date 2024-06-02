#include <Arduino.h>

#define RF_DIGITAL_HIGH D5
#define RF_DIGITAL_LOW D4
#define MENU_BUTTON_1 D9
#define MENU_BUTTON_2 D8
#define MENU_BUTTON_3 D7
#define MENU_BUTTON_4 D6

class RFReceiver {
    private:
        uint8_t lastState4b = 0b0000;
    public:
        RFReceiver();
        ~RFReceiver();
        void setup();
        uint8_t tick();
};

