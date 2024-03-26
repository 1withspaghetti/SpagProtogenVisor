#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_SSD1306.h>

#include <Faces.h>
#include <Icons.h>

// ############################
// #  Functions Declarations  #
// ############################
void drawProtoIcon(int, int, int);
void fillHeadphoneMatrix(uint16_t);
float fract(float);
float mix(float, float, float);
float* hsv2rgb(float, float, float, float*);


// ############################
// #      Pin Constants       #
// ############################

// Neo Matrix Constants
#define NEO_MATRIX_DATA_PIN 3
#define NEO_MATRIX_WIDTH 48
#define NEO_MATRIX_HEIGHT 8

#define NEO_MATRIX_BRIGHTNESS_MULTIPLIER 3 // Brightness can be changed from 1-10 times this number

#define HEADPHONE_NEO_MATRIX_DATA_PIN 11
#define HEADPHONE_NEO_MATRIX_WIDTH 16
#define HEADPHONE_NEO_MATRIX_HEIGHT 8

// Microphone Constants
#define MICROPHONE_PIN A6

// Receiver Constants
#define RF_DIGITAL_HIGH 5
#define RF_DIGITAL_LOW 4
#define MENU_BUTTON_1 9
#define MENU_BUTTON_2 8
#define MENU_BUTTON_3 7
#define MENU_BUTTON_4 6

// OLED Constants
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

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
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(NEO_MATRIX_WIDTH, NEO_MATRIX_HEIGHT, NEO_MATRIX_DATA_PIN,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB + NEO_KHZ800);

// Neo matrix object for headphones
Adafruit_NeoMatrix headphone_matrix = Adafruit_NeoMatrix(HEADPHONE_NEO_MATRIX_WIDTH, HEADPHONE_NEO_MATRIX_HEIGHT, HEADPHONE_NEO_MATRIX_DATA_PIN,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB + NEO_KHZ800);

// OLED Display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


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
uint16_t color = matrix.Color(0,170,255);
uint16_t headphone_color = matrix.Color(0,50,255);
uint16_t error_color = matrix.Color(255,0,0);
uint16_t blush_color = matrix.Color(255, 43, 85);

int emotion = 0;
int emotionChange = -1;
int blink = 100;

int mic_max_diff = 0;
int mic_avg_time = 0;
long mic_avg_total = 0;
int mic_triggers = 0;
bool mic_active = false;
bool mic_was_active = false;

uint8_t visualizerBars[FACE_WIDTH];
const uint16_t visualizerColors[] = {matrix.Color(255, 0, 0), matrix.Color(255, 78, 0), matrix.Color(255, 156, 0), matrix.Color(255, 234, 0), matrix.Color(198, 255, 0), matrix.Color(120, 255, 0), matrix.Color(42, 255, 0), matrix.Color(0, 255, 0)};
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

  // NEO Matrix
  matrix.begin();
  matrix.setBrightness(brightness * NEO_MATRIX_BRIGHTNESS_MULTIPLIER);

  headphone_matrix.begin();
  headphone_matrix.setBrightness(7);
  fillHeadphoneMatrix(headphone_color);

  // OLED Display Setup
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed :c"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();

  Serial.println("Protogen started! ^w^");
}


// ############################
// #          Loop            #
// ############################

void loop() {
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

  // Blinks
  int eye_frame = EYE_FRAMES-1-min(abs(blink), EYE_FRAMES-1);
  if (eye_frame == 2 && emotionChange != -1) {
    
    // Handle headphone change on error emotion
    if (emotion == 6 && emotionChange != 6) fillHeadphoneMatrix(headphone_color);
    if (emotionChange == 6 && emotion != 6) fillHeadphoneMatrix(error_color);

    emotion = emotionChange;
    emotionChange = -1;
  }
  if (abs(blink) == EYE_FRAMES-1 || blink == 0) hasChange = true;
  blink--;
  if (blink <= -EYE_FRAMES) blink = random(100,150);

  // Main Face
  matrix.fillScreen(0);

  // Set color
  hsv2rgb(hue, 1.0, 1.0, col);
  color = matrix.Color(col[0]*255,col[1]*255,col[2]*255);
  //Serial.println("R: "+String(col[0])+" G: "+String(col[1])+" B: "+String(col[2])+" Hue: "+String(hue));

  // Override for error emotion
  if (emotion == 6) color = error_color;

  // Audio Visualizer
  if (emotion == 7) {
    float new_offset = (mic_max_diff - mic_avg_total / mic_avg_time) / 5.0;
    if (new_offset > offset) offset = new_offset; // If the new value is bigger, set it
    else offset = (offset * 2 + new_offset) / 3; // else, slowly decrease to new value
    offset = constrain(offset, 0, 4);
    //Serial.println("offset:"+String(offset));
    for (int i = 0; i < FACE_WIDTH; i++) {
      uint8_t newVal = visualizerBars[i] + (random(3)-1);
      visualizerBars[i] = constrain(newVal, 1, 4);
      for (int y = 0; y < FACE_HEIGHT; y++) {
        if (y >= FACE_HEIGHT - visualizerBars[i] - offset) matrix.fillRect(EYE_WIDTH + i*2, y, 2, 1, visualizerColors[y]);
      }
    }
  }
  else if (emotion == 5) {} // No face on blush
   else {
    // Face
    for (int y = 0; y < FACE_HEIGHT; y++) {
      uint16_t row = pgm_read_word(&((!mic_active ? face : face_talking)[getMouthIndex(emotion)][y]));
      for (int x = 0; x < FACE_WIDTH; x++) {
        if (row >> (FACE_WIDTH-1 - x) & 0b1 == 1) {
          matrix.drawPixel(EYE_WIDTH + x, y, color);
          matrix.drawPixel((EYE_WIDTH + (FACE_WIDTH * 2)) - 1 - x, y, color);
        }
      }
    }
  }
  // If blush should be applied
  if (emotion == 5) {
    for (int y = 0; y < FACE_HEIGHT; y++) {
      uint16_t row = pgm_read_word(&(face_blush[y]));
      for (int x = 0; x < FACE_WIDTH; x++) {
        if (row >> (FACE_WIDTH-1 - x) & 0b1 == 1) {
          matrix.drawPixel(EYE_WIDTH + x, y, blush_color);
          matrix.drawPixel((EYE_WIDTH + (FACE_WIDTH * 2)) - 1 - x, y, blush_color);
        }
      }
    }
  }

  // Eyes
  for (int y = 0; y < EYE_HEIGHT; y++) {
    uint8_t row = pgm_read_byte(&(eye[getEyeIndex(emotion)][eye_frame][y]));
    for (int x = 0; x < EYE_WIDTH; x++) {
      if (row >> (EYE_WIDTH-1 - x) & 0b1 == 1) {
        matrix.drawPixel(x, y, color);
        matrix.drawPixel(NEO_MATRIX_WIDTH-1-x, y, color);
      }
    }
  }

  matrix.show();

  if (hasChange) {
    display.clearDisplay();

    // Add text
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("Spag!");

    // Proto Display
    display.drawBitmap(0, 16, proto_display_bmp, PROTO_DISPLAY_WIDTH, PROTO_DISPLAY_HEIGHT, SSD1306_WHITE);
    // Current Face
    for (int y = 0; y < FACE_HEIGHT; y++) {
      uint16_t row = pgm_read_word(&((!mic_active ? face : face_talking)[getMouthIndex(emotion)][y]));
      for (int x = 0; x < FACE_WIDTH; x++) {
        if (row >> (FACE_WIDTH-1 - x) & 0b1 == 1) {
          display.fillRect(16 + x*2, 16 + 26 + y*2, 2, 2, SSD1306_WHITE);
        }
      }
    }
    // Current Eyes
    for (int y = 0; y < EYE_HEIGHT; y++) {
      uint16_t row = pgm_read_byte(&(eye[getEyeIndex(emotion)][eye_frame][y]));
      for (int x = 0; x < EYE_WIDTH; x++) {
        if (row >> (EYE_WIDTH-1 - x) & 0b1 == 1) {
          display.fillRect(13 + x*2, 16 + 15 + y*2, 2, 2, SSD1306_WHITE);
        }
      }
    }

    // Brightness Indicator
    display.drawBitmap(64 + 1, 16 + 31, brightness_bmp, BRIGHTNESS_BMP_WIDTH, BRIGHTNESS_BMP_HEIGHT, SSD1306_WHITE);
    for (int i = 0; i < brightness; i++) {
      display.drawRect(64 + 3, 16 + 27 - i*3, 9, 2, SSD1306_WHITE);
    }

    // Menu Outline
    display.drawRect(64 + 20, 16 + 1, 21, 21, SSD1306_WHITE);
    display.drawRect(64 + 42, 16 + 1, 21, 21, SSD1306_WHITE);
    display.drawRect(64 + 20, 16 + 23, 21, 21, SSD1306_WHITE);
    display.drawRect(64 + 42, 16 + 23, 21, 21, SSD1306_WHITE);

    // Menu Button
    if (menu < 3) {
      display.drawRect(64 + 46, 16 + 27, 13, 3, SSD1306_WHITE);
      display.drawRect(64 + 46, 16 + 32, 13, 3, SSD1306_WHITE);
      display.drawRect(64 + 46, 16 + 37, 13, 3, SSD1306_WHITE);
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
      display.drawBitmap(64 + 21, 16 + 24, setting_icon_bmp, ICON_BMP_WIDTH, ICON_BMP_HEIGHT, SSD1306_WHITE);
    } else if (menu == 3) {
      display.drawBitmap(64 + 21, 16 + 2, brightness_up_icon_bmp, ICON_BMP_WIDTH, ICON_BMP_HEIGHT, SSD1306_WHITE);
      display.drawBitmap(64 + 43, 16 + 2, brightness_down_icon_bmp, ICON_BMP_WIDTH, ICON_BMP_HEIGHT, SSD1306_WHITE);
      display.drawBitmap(64 + 21, 16 + 24, reset_icon_bmp, ICON_BMP_WIDTH, ICON_BMP_HEIGHT, SSD1306_WHITE);
      display.drawBitmap(64 + 43, 16 + 24, back_icon_bmp, ICON_BMP_WIDTH, ICON_BMP_HEIGHT, SSD1306_WHITE);
    }

    display.display();
  }

  // Increment vars
  hasChange = false;

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
        emotionChange = newEmotion;
        blink = EYE_FRAMES-1;
      } else {
        hasChange = true; // Only non-emotion changes need to be updated immediatly, emotions will update during blink
      }
      brightness = constrain(brightness, 1, 10);
      matrix.setBrightness(brightness * NEO_MATRIX_BRIGHTNESS_MULTIPLIER);
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

// ############################
// #     Helper Functions     #
// ############################

void fillHeadphoneMatrix(uint16_t c) {
  headphone_matrix.fillScreen(0);
  for (int y = 0; y < HEADPHONE_HEIGHT; y++) {
    uint8_t row = pgm_read_byte(&(headphone_ico[y]));
    for (int x = 0; x < HEADPHONE_WIDTH; x++) {
      if (row >> (HEADPHONE_WIDTH-1 - x) & 0b1 == 1) {
        headphone_matrix.drawPixel(x, y, c);
        headphone_matrix.drawPixel(x + HEADPHONE_WIDTH, y, c);
      }
    }
  }
  headphone_matrix.show();
}

void drawProtoIcon(int rel_x, int rel_y, int emotionIcon) {
  // Face
  for (int y = 0; y < FACE_HEIGHT; y++) {
    uint16_t row = pgm_read_word(&(face[getMouthIndex(emotionIcon)][y]));
    for (int x = 0; x < FACE_WIDTH; x++) {
      if (row >> (FACE_WIDTH-1 - x) & 0b1 == 1) {
        display.drawPixel(rel_x + 3 + x, rel_y + 10 + y, SSD1306_WHITE);
      }
    }
  }

  // Blush
  if (emotionIcon == 5) {
    for (int y = 0; y < FACE_HEIGHT; y++) {
      uint16_t row = pgm_read_word(&(face_blush[y]));
      for (int x = 0; x < FACE_WIDTH; x++) {
        if (row >> (FACE_WIDTH-1 - x) & 0b1 == 1) {
          display.drawPixel(rel_x + 3 + x, rel_y + 10 + y, SSD1306_WHITE);
        }
      }
    }
  }

  // Eyes
  for (int y = 0; y < EYE_HEIGHT; y++) {
    uint16_t row = pgm_read_byte(&(eye[getEyeIndex(emotionIcon)][0][y]));
    for (int x = 0; x < EYE_WIDTH; x++) {
      if (row >> (EYE_WIDTH-1 - x) & 0b1 == 1) {
        display.drawPixel(rel_x + 0 + x, rel_y + 1 + y, SSD1306_WHITE);
      }
    }
  }
}


// Math functions I totally didn't steal from stack overflow

float fract(float x) { return x - int(x); }

float mix(float a, float b, float t) { return a + (b - a) * t; }

float* hsv2rgb(float h, float s, float b, float* rgb) {
  rgb[0] = b * mix(1.0, constrain(fabs(fract(h + 1.0) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s);
  rgb[1] = b * mix(1.0, constrain(fabs(fract(h + 0.6666666) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s);
  rgb[2] = b * mix(1.0, constrain(fabs(fract(h + 0.3333333) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s);
  return rgb;
}