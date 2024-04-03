#include <Arduino.h>

#include "util/ColorUtil.h"
#include "components/FaceMatrix.h"
#include "components/HeadphoneMatrix.h"
#include "components/OLEDDisplay.h"
#include "components/EmotionManager.h"


// ############################
// #      Pin Constants       #
// ############################

// Microphone Constants
#define MICROPHONE_PIN A7

// Receiver Constants
#define RF_DIGITAL_HIGH 5
#define RF_DIGITAL_LOW 4
#define MENU_BUTTON_1 D6
#define MENU_BUTTON_2 D7
#define MENU_BUTTON_3 D8
#define MENU_BUTTON_4 D9

// ############################
// #     Other Constants      #
// ############################

// Color cycle constants
#define HUE_START 0.555
#define HUE_END 0.700
#define HUE_CHANGE 0.003

// Mic constants
#define MIC_TRIGGER 6 // The baseline difference to active talking
#define MIC_TRIGGER_PERIOD 4
#define MIC_AVERAGE_RESET 600


// ############################
// #     Object Creation      #
// ############################

// Reset function at address 0
void(* resetFunc) (void) = 0;

// Neo matrix object for face
FaceMatrix face_matrix = FaceMatrix(4);

// Neo matrix object for headphones
HeadphoneMatrix headphone_matrix = HeadphoneMatrix();

// OLED Display object
OLEDDisplay display = OLEDDisplay();

// Emotion Manager object
EmotionManager emotion = EmotionManager(0);


// ############################
// #     Global Variables     #
// ############################

int menu = 0;
bool buttonPressed = false;

bool hasChange = true;

uint8_t brightness = 4;

bool hue_up = true;
float hue = HUE_START;
float col[3] = {0,50,255};
uint16_t color = Adafruit_NeoMatrix::Color(0,170,255);
uint16_t headphone_color = Adafruit_NeoMatrix::Color(0,50,255);
uint16_t error_color = Adafruit_NeoMatrix::Color(255,0,0);

int mic_max_diff = 0;
int mic_avg_time = 0;
long mic_avg_total = 0;
int mic_triggers = 0;
bool mic_active = false;
bool mic_was_active = false;

float offset = 0.0;


// ############################
// #          Setup           #
// ############################

void setup() {
  Serial.begin(9600);

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

  // Update face vectors
  emotion.tick();


  // Handle Microphone
  /*Serial.print("diff:");
  Serial.print(mic_max_diff);
  Serial.print(" avg:");
  Serial.print((float)mic_avg_total / mic_avg_time);
  Serial.print(" baseline:");
  Serial.println((float)mic_avg_total / mic_avg_time + MIC_TRIGGER);*/
  if (mic_max_diff >= (float)mic_avg_total / mic_avg_time + MIC_TRIGGER) {
    mic_triggers++;
  }
  if (mic_avg_time % MIC_TRIGGER_PERIOD == 0) {
    mic_active = mic_triggers >= (MIC_TRIGGER_PERIOD / 2);
    mic_triggers = 0;

    if (mic_active != mic_was_active) hasChange = true;
    mic_was_active = mic_active;

    //if (mic_active) Serial.println("Mic Active");
    //else Serial.println("");
  }
  if (mic_avg_time >= MIC_AVERAGE_RESET) {
    mic_avg_total = 0;
    mic_avg_time = 0;
  }


  // Face color
  hsv2rgb(hue, 1.0, 1.0, col);
  color = Adafruit_NeoMatrix::Color(col[0]*255,col[1]*255,col[2]*255);
  //Serial.println("R: "+String(col[0])+" G: "+String(col[1])+" B: "+String(col[2])+" Hue: "+String(hue));

  // Override for error emotion
  if (emotion.getEmotion() == 6) color = error_color;

  // Audio Visualizer
  if (emotion.getEmotion() == 7) {
    float new_offset = (mic_max_diff - mic_avg_total / mic_avg_time) / 5.0;
    if (new_offset > offset) offset = new_offset; // If the new value is bigger, set it
    else offset = (offset * 2 + new_offset) / 3; // else, slowly decrease to new value
    offset = constrain(offset, 0, 4);
  }
  

  face_matrix.display(color, emotion.getEyeVector(), emotion.getMouthVector());


  if (hasChange) {
    display.render(brightness, mic_active, emotion.getEyeVector(), emotion.getMouthVector());
    hasChange = false;
  }



  // Increment Vars

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

  //Serial.println("A:"+String(digitalRead(MENU_BUTTON_1))+" B:"+digitalRead(MENU_BUTTON_2)+" C:"+digitalRead(MENU_BUTTON_3)+" D:"+digitalRead(MENU_BUTTON_4));
  if (!buttonPressed) {
    int newEmotion = -1;
    if (digitalRead(MENU_BUTTON_1) == 1) {
      buttonPressed = true;
      if (menu == 0) newEmotion = 0;
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
      else if (menu == 3) resetFunc();
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
        if (newEmotion == 6) headphone_matrix.display(error_color);
        else if (emotion.getEmotion() == 6) headphone_matrix.display(headphone_color);
        
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

  // 50millis delay with checking mic every millis
  mic_max_diff = 0;

  unsigned long start = millis();
  int mic_min = 1000;
  int mic_max = 0;
  // Sub-tick loop, runs for 50 millis between each tick
  while (start + 50 > millis()) {
    // Manage mic input
    int val = analogRead(MICROPHONE_PIN);
    //Serial.println("val:"+String(val)+" min:100 max:300"); // Potentiometer WAS line parallel to the board averaging about 700 val
    mic_max = max(val, mic_max);
    mic_min = min(val, mic_min);
  }
  mic_max_diff = mic_max - mic_min;
  //Serial.println("diff:"+String(mic_max_diff));

  mic_avg_total += mic_max_diff;
  mic_avg_time++;
}