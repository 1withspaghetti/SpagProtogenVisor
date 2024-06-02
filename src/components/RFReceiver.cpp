#include "RFReceiver.h"

RFReceiver::RFReceiver() {};

RFReceiver::~RFReceiver() {};

void RFReceiver::setup() {
    pinMode(RF_DIGITAL_HIGH, OUTPUT);
    pinMode(RF_DIGITAL_LOW, OUTPUT);
    pinMode(MENU_BUTTON_1, INPUT);
    pinMode(MENU_BUTTON_2, INPUT);
    pinMode(MENU_BUTTON_3, INPUT);
    pinMode(MENU_BUTTON_4, INPUT);

    digitalWrite(RF_DIGITAL_HIGH, HIGH);
    digitalWrite(RF_DIGITAL_LOW, LOW);
}

/**
 * This function is used to check if any of the menu buttons have been pressed since last tick.
 * The last 4 bits of the return value represents a button each, from left to right, 1-4.
 * @return the 4 bit number
*/
uint8_t RFReceiver::tick() {
    
    //Serial.println("A:"+String(digitalRead(MENU_BUTTON_1))+" B:"+String(digitalRead(MENU_BUTTON_2))+" C:"+String(digitalRead(MENU_BUTTON_3))+" D:"+String(digitalRead(MENU_BUTTON_4)));

    bool state1 = digitalRead(MENU_BUTTON_1) == HIGH;
    bool state2 = digitalRead(MENU_BUTTON_2) == HIGH;
    bool state3 = digitalRead(MENU_BUTTON_3) == HIGH;
    bool state4 = digitalRead(MENU_BUTTON_4) == HIGH;

    uint8_t newState4b = (state1 << 3) | (state2 << 2) | (state3 << 1) | state4;

    uint8_t diff4b = newState4b & ~lastState4b;

    lastState4b = newState4b;

    return diff4b;
}