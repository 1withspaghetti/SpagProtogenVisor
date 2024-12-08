#include <Arduino.h>

#include "settings.h"
#include "components/FaceMatrix.h"
#include "components/HeadphoneMatrix.h"
#include "components/OLEDDisplay.h"
#include "components/EmotionManager.h"
#include "components/RFReceiver.h"
#include "components/Microphone.h"

// ############################
// #        CONSTANTS         #
// ############################

#define TICKS_PER_SECOND 20

// Color cycle constants
#define HUE_START 142
#define HUE_END 167
#define HUE_CHANGE_PER_SECOND 6

// Brightness Constants
#define BRIGHTNESS_INITIAL 10 // Brightness can be changed from 1-10 times BRIGHTNESS_MULTIPLIER plus this number
#define BRIGHTNESS_MULTIPLIER 12 // Brightness can be changed from 1-10 times this number plus BRIGHTNESS_INITIAL

// ############################
// #     Global Variables     #
// ############################

long microsLastLoop = 0;

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

// Microphone object
Microphone mic = Microphone();


// ############################
// #          Setup           #
// ############################

void setup() {
  Serial.begin(115200);

  randomSeed(analogRead(0));

  // Emotion Manager
  emotion.setup();

  // NEO Matrixes
  face_matrix.setup();
  headphone_matrix.setup();

  // OLED Display
  display.setup();

  // RF Receiver
  rf.setup();

  // Microphone
  mic.setup();
  xTaskCreatePinnedToCore(
    [] (void* pvParameters) {
      for (;;) {
        mic.tick();
        vTaskDelay(50);
      }
    },
    "Microphone",
    10000,
    NULL,
    1,
    NULL,
    0
  );

  Serial.println("Protogen started! ^w^");
}


// ############################
// #          Loop            #
// ############################

void loop() {
  #ifdef VERBOSE_TICK_TIME
  unsigned long loopStart = millis();
  #endif

  // Tick, only runs TICKS_PER_SECOND times per second but renders every loop
  if (micros() - microsLastLoop > 1000000 / TICKS_PER_SECOND) {
    microsLastLoop = micros();

    #ifdef VERBOSE_TICK_TIME
    unsigned long tickStart = millis();
    #endif
    tick();
    #ifdef VERBOSE_TICK_TIME
    Serial.print("  Tick Time: ");
    Serial.print(millis()-tickStart);
    Serial.println("ms");
    #endif
  }

  // Delta is the percentage of the time through the current tick for this render frame
  float delta = (micros() - microsLastLoop) / (1000.0 / TICKS_PER_SECOND);
  render(delta);


  #ifdef VERBOSE_TICK_TIME
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
  #ifdef VERBOSE_RENDER_TIME
  unsigned long matrixStart = millis();
  #endif
  color.setHue((uint8_t) (sin(millis() / (1000.0 * 2 * PI) * HUE_CHANGE_PER_SECOND) * hueRadius + hueCenter));
  bool wasFaceMatrixRecalculated = face_matrix.display(color, brightness * BRIGHTNESS_MULTIPLIER + BRIGHTNESS_INITIAL, emotion.getEyeVector(), emotion.getMouthVector(), emotion.getSpecialVector());
  if (wasFaceMatrixRecalculated) display.setNeedsRender(true);
  headphone_matrix.display(color, brightness * BRIGHTNESS_MULTIPLIER + BRIGHTNESS_INITIAL);
  #ifdef VERBOSE_RENDER_TIME
  Serial.print("  Matrix Render Time: ");
  Serial.print(millis()-matrixStart);
  Serial.println("ms");
  #endif


  // Render OLED display
  #ifdef VERBOSE_RENDER_TIME
  unsigned long oledStart = millis();
  #endif
  // Note: The OLED display is only updated if the display needs to be rendered, by calling setNeedsRender(true)
  // Overwise, this function will return immediately
  display.render(brightness, emotion.getEyeVector(), emotion.getMouthVector());
  #ifdef VERBOSE_RENDER_TIME
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
  double mic_magnitude = mic.getMagnitude();
  #ifdef VERBOSE_MIC
  Serial.print("Mic Magnitude: ");
  Serial.println(mic_magnitude);
  #endif
  emotion.tick(mic_magnitude);

  // Check for button presses on the remote
  uint8_t buttonState = rf.tick();
  if (buttonState > 0) {
    int menu = display.getMenu();
    if (menu == 0) {
        if      (buttonState & 0b1000) emotion.setEmotion(0);
        else if (buttonState & 0b0100) emotion.setEmotion(1);
        else if (buttonState & 0b0010) emotion.setEmotion(2);
        else if (buttonState & 0b0001) display.setMenu(1);
    } else if (menu == 1) {
        if      (buttonState & 0b1000) emotion.setEmotion(3);
        else if (buttonState & 0b0100) emotion.setEmotion(4);
        else if (buttonState & 0b0010) emotion.setEmotion(5);
        else if (buttonState & 0b0001) display.setMenu(2);
    } else if (menu == 2) {
        if      (buttonState & 0b1000) emotion.setEmotion(6);
        else if (buttonState & 0b0100) emotion.setEmotion(7);
        else if (buttonState & 0b0010) display.setMenu(3);
        else if (buttonState & 0b0001) display.setMenu(0);
    } else if (menu == 3) {
        if      (buttonState & 0b1000) {brightness--; display.setNeedsRender(true);}
        else if (buttonState & 0b0100) {brightness++; display.setNeedsRender(true);}
        else if (buttonState & 0b0010) resetFunc();
        else if (buttonState & 0b0001) display.setMenu(0);
    }

    brightness = constrain(brightness, 0, 10);

    #ifdef VERBOSE_RF
    Serial.print("RF Signal: ");
    Serial.print(buttonState, BIN);
    Serial.print(" | Menu: ");
    Serial.println(menu);
    #endif
  }
}