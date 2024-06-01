#include <Arduino.h>

#include "components/FaceMatrix.h"
#include "components/HeadphoneMatrix.h"
#include "components/OLEDDisplay.h"
#include "components/EmotionManager.h"

// ############################
#define DEBUG
// ############################

#define TICKS_PER_SECOND 20

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
#define HUE_CHANGE_PER_SECOND 6

// ############################
// #     Global Variables     #
// ############################

long microsLastLoop = 0;

int menu = 0;
bool buttonPressed = false;

bool shouldPaintOLED = true;

uint8_t brightness = 5;

const float hueRadius = (HUE_END - HUE_START) / 2;
const float hueCenter = HUE_START + hueRadius;

CRGB color = CHSV(HUE_END, 255, 255);


// ############################
// #   Function Prototypes    #
// ############################

void render(float delta);
void tick();

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
  unsigned long loopStart = millis();
  #endif

  // Tick, only runs TICKS_PER_SECOND times per second but renders every loop
  if (micros() - microsLastLoop > 1000000 / TICKS_PER_SECOND) {
    microsLastLoop = micros();

    #ifdef DEBUG
    unsigned long tickStart = millis();
    #endif
    tick();
    #ifdef DEBUG
    Serial.print("  Tick Time: ");
    Serial.print(millis()-tickStart);
    Serial.println("ms");
    #endif
  }

  // Delta is the percentage of the time through the current tick for this render frame
  float delta = (micros() - microsLastLoop) / (1000.0 / TICKS_PER_SECOND);
  render(delta);


  #ifdef DEBUG
  unsigned long loopEnd = millis();
  Serial.print("Loop Time: ");
  Serial.print(loopEnd - loopStart);
  Serial.print("ms | FPS: ");
  Serial.println(1000.0 / (loopEnd - loopStart), 2);
  #endif
}


// ############################
// #         Render           #
// ############################

void render(float delta) {
  
  // Render face matrix
  #ifdef DEBUG
  unsigned long matrixStart = millis();
  #endif
  color.setHue((uint8_t) (sin(millis() / (1000.0 * 2 * PI) * HUE_CHANGE_PER_SECOND) * hueRadius + hueCenter));
  shouldPaintOLED = shouldPaintOLED || face_matrix.display(color, emotion.getEyeVector(), emotion.getMouthVector());
  #ifdef DEBUG
  Serial.print("  Matrix Render Time: ");
  Serial.print(millis()-matrixStart);
  Serial.println("ms");
  #endif


  // Render OLED display
  #ifdef DEBUG
  unsigned long oledStart = millis();
  #endif
  if (shouldPaintOLED) {
    display.render(menu, brightness, false, emotion.getEyeVector(), emotion.getMouthVector());
    shouldPaintOLED = false;
  }
  #ifdef DEBUG
  Serial.print("  OLED Render Time: ");
  Serial.print(millis()-oledStart);
  Serial.println("ms");
  #endif

}


// ############################
// #          Tick            #
// ############################

void tick() {

  // Update face vectors
  emotion.tick();

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
        shouldPaintOLED = true; // Only non-emotion changes need to be updated immediatly, emotions will update during blink
      }
      brightness = constrain(brightness, 1, 10);
      face_matrix.setBrightness(brightness);
    }
  } else {
    if (digitalRead(MENU_BUTTON_1) + digitalRead(MENU_BUTTON_2) + digitalRead(MENU_BUTTON_3) + digitalRead(MENU_BUTTON_4) == 0) {
      buttonPressed = false;
    }
  }
}