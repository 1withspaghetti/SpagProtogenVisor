#include <Arduino.h>

#include "components/FaceMatrix.h"
#include "components/HeadphoneMatrix.h"
#include "components/OLEDDisplay.h"
#include "components/EmotionManager.h"

// ############################
#define DEBUG
// ############################


// ############################
// #      Pin Constants       #
// ############################

// Microphone Constants
#define MICROPHONE_PIN A7

// Receiver Constants
#define RF_DIGITAL_HIGH 5
#define RF_DIGITAL_LOW 4
#define MENU_BUTTON_1 D9
#define MENU_BUTTON_2 D8
#define MENU_BUTTON_3 D7
#define MENU_BUTTON_4 D6

// ############################
// #     Other Constants      #
// ############################

// Color cycle constants
#define HUE_START 142
#define HUE_END 167
#define HUE_CHANGE 0.5

// Mic constants
#define MIC_TRIGGER 6 // The baseline difference to active talking
#define MIC_TRIGGER_PERIOD 4
#define MIC_AVERAGE_RESET 600


// ############################
// #     Global Variables     #
// ############################

int menu = 0;
bool buttonPressed = false;

bool hasChange = true;

uint8_t brightness = 8;

bool hue_up = true;
float hue = HUE_START;

CRGB color = CHSV(hue * 255, 255, 255);


// ############################
// #     Object Creation      #
// ############################

// Reset function at address 0
void(* resetFunc) (void) = 0;

// Neo matrix object for face
FaceMatrix face_matrix = FaceMatrix(brightness);

// Neo matrix object for headphones
HeadphoneMatrix headphone_matrix = HeadphoneMatrix();

// OLED Display object
OLEDDisplay display = OLEDDisplay();

// Emotion Manager object
EmotionManager emotion = EmotionManager(0);


// ############################
// #          Setup           #
// ############################

void setup() {
  Serial.begin(115200);

  pinMode(RF_DIGITAL_HIGH, OUTPUT);
  pinMode(RF_DIGITAL_LOW, OUTPUT);

  pinMode(MENU_BUTTON_1, INPUT);
  pinMode(MENU_BUTTON_2, INPUT);
  pinMode(MENU_BUTTON_3, INPUT);
  pinMode(MENU_BUTTON_4, INPUT);

  digitalWrite(RF_DIGITAL_HIGH, HIGH);
  digitalWrite(RF_DIGITAL_LOW, LOW);

  randomSeed(analogRead(0));

  // NEO Matrixes
  face_matrix.setup();
  headphone_matrix.setup();

  // OLED Display
  display.setup();

  Serial.println("Protogen started! ^w^");
}


// ############################
// #          Loop            #
// ############################

void loop() {
  #ifdef DEBUG
  long tickStart = millis();
  #endif

  // Update face vectors
  emotion.tick();

  // Face color
  if (hue_up) {
    hue += HUE_CHANGE;
    if (hue >= HUE_END) {
      hue = HUE_END;
      hue_up = false;
    }
  } else {
    hue -= HUE_CHANGE;
    if (hue <= HUE_START) {
      hue = HUE_START;
      hue_up = true;
    }
  }
  color.setHue((uint8_t) hue);



  // Display face matrix
  #ifdef DEBUG
  long matrixStart = millis();
  #endif
  face_matrix.display(color, emotion.getEyeVector(), emotion.getMouthVector());
  #ifdef DEBUG
  Serial.println("  Matrix Render Time: "+String(millis()-matrixStart));
  #endif

  // If needed, update OLED display
  // if (hasChange) {
  //   Serial.println("Updating Display");
  //   display.render(menu, brightness, mic_active, emotion.getEyeVector(), emotion.getMouthVector());
  //   hasChange = false;
  // }

  // Update the OLED display
  #ifdef DEBUG
  long oledStart = millis();
  #endif
  display.render(menu, brightness, false, emotion.getEyeVector(), emotion.getMouthVector());
  #ifdef DEBUG
  Serial.println("  OLED Render Time: "+String(millis()-oledStart));
  #endif


  //Serial.println("A:"+String(digitalRead(MENU_BUTTON_1))+" B:"+digitalRead(MENU_BUTTON_2)+" C:"+digitalRead(MENU_BUTTON_3)+" D:"+digitalRead(MENU_BUTTON_4));
  if (!buttonPressed) {
    int newEmotion = -1;
    if (digitalRead(MENU_BUTTON_1) == 1) {
      buttonPressed = true;
      if (menu == 0) newEmotion = (emotion.getEmotion() + 1) % 6;
      else if (menu == 1) newEmotion = 3;
      else if (menu == 2) newEmotion = 6;
      else if (menu == 3) brightness++;
    } else if (digitalRead(MENU_BUTTON_2) == 1) {
      buttonPressed = true;
      if (menu == 0) newEmotion = 1;
      else if (menu == 1) newEmotion = 4;
      else if (menu == 2) newEmotion = 7;
      else if (menu == 3) brightness--;
    } else if (digitalRead(MENU_BUTTON_3) == 1) {
      buttonPressed = true;
      if (menu == 0) newEmotion = 2;
      else if (menu == 1) newEmotion = 5;
      else if (menu == 2) menu = 3;
      else if (menu == 3) /*resetFunc();*/ Serial.println("Reset");
    } else if (digitalRead(MENU_BUTTON_4) == 1) {
      buttonPressed = true;
      if (menu == 0) menu = 1;
      else if (menu == 1) menu = 2;
      else if (menu == 2) menu = 0;
      else if (menu == 3) menu = 0;
    } else {
      buttonPressed = false;
    }
    if (buttonPressed) {
      if (newEmotion > -1) {
        // Handle headphone matrix changing color when error emotion is entered/left
        // TODO: Update this with fastled
        
        emotion.setEmotion(newEmotion);
      } else {
        hasChange = true; // Only non-emotion changes need to be updated immediatly, emotions will update during blink
      }
      brightness = constrain(brightness, 1, 10);
      face_matrix.setBrightness(brightness);
    }
  } else {
    if (digitalRead(MENU_BUTTON_1) + digitalRead(MENU_BUTTON_2) + digitalRead(MENU_BUTTON_3) + digitalRead(MENU_BUTTON_4) == 0) {
      buttonPressed = false;
    }
  }

  #ifdef DEBUG
  long tickEnd = millis();
  Serial.print("Tick Time: ");
  Serial.print(tickEnd - tickStart, 4);
  Serial.print("ms | TPS: ");
  Serial.println(1000.0 / (tickEnd - tickStart), 2);
  #endif
}