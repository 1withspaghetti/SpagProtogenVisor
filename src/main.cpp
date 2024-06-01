#include <Arduino.h>

#include "components/FaceMatrix.h"
#include "components/HeadphoneMatrix.h"
#include "components/OLEDDisplay.h"
#include "components/EmotionManager.h"
#include "components/RFReceiver.h"

// ############################
#define DEBUG
// ############################


// ############################
// #        CONSTANTS         #
// ############################

#define TICKS_PER_SECOND 20

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

uint8_t brightness = 3;

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

// RF Receiver object
RFReceiver rf = RFReceiver();

// Emotion Manager object
EmotionManager emotion = EmotionManager(0);


// ############################
// #          Setup           #
// ############################

void setup() {
  Serial.begin(115200);

  randomSeed(analogRead(0));

  // NEO Matrixes
  face_matrix.setup();
  headphone_matrix.setup();

  // OLED Display
  display.setup();

  // RF Receiver
  rf.setup();

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

  // Check for button presses on the remote
  uint8_t buttonState = rf.tick();
  if (buttonState > 0) {
    if (menu == 0) {
        if      (buttonState & 0b1000) emotion.setEmotion((emotion.getEmotion() + 1) % 6);
        else if (buttonState & 0b0100) emotion.setEmotion(0);
        else if (buttonState & 0b0010) emotion.setEmotion(0);
        else if (buttonState & 0b0001) {menu = 1; shouldPaintOLED = true;}
    } else if (menu == 1) {
        if      (buttonState & 0b1000) emotion.setEmotion(0);
        else if (buttonState & 0b0100) emotion.setEmotion(0);
        else if (buttonState & 0b0010) emotion.setEmotion(0);
        else if (buttonState & 0b0001) {menu = 2; shouldPaintOLED = true;}
    } else if (menu == 2) {
        if      (buttonState & 0b1000) emotion.setEmotion(0);
        else if (buttonState & 0b0100) emotion.setEmotion(0);
        else if (buttonState & 0b0010) {menu = 3; shouldPaintOLED = true;}
        else if (buttonState & 0b0001) {menu = 2; shouldPaintOLED = true;}
    } else if (menu == 3) {
        if      (buttonState & 0b1000) menu = brightness++;
        else if (buttonState & 0b0100) menu = brightness--;
        else if (buttonState & 0b0010) resetFunc();
        else if (buttonState & 0b0001) {menu = 0; shouldPaintOLED = true;}
    }

    brightness = constrain(brightness, 0, 10);
    face_matrix.setBrightness(brightness);
  }
}