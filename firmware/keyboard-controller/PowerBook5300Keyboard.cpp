// I don't know why I need to define USBCON. My board is definitely supported.
// I have a Sparkfun Pro Micro, which is Leonardo compatible. Yet, I get an error
// that my board is not supported if I don't define USBCON. I even tried with an
// actual Leonrdo board, and I have the same problem. So, I will leave this define
// here until I understand what is going wrong. With the define, it works fine.
#define USBCON
// Even if US layout is the default, you get a compiler warning if you don't define it.
#define HID_CUSTOM_LAYOUT
#define LAYOUT_UNITED_KINGDOM
//#define LAYOUT_US_ENGLISH
#include <HID-Project.h>

#include "PowerBook5300Keyboard.h"
#include "InputOutputTools.h"
//#include "KeyCodes.h"


#define DEBUG

static void debugPrint(String s, uint8_t row, uint8_t column, uint8_t keyCode) {
#ifdef DEBUG
  char c = keyCode;
  String str = s + "row=" + row + ", column=" + column + ", 0x" + String(keyCode, HEX) + ", " + String(c) + "\n";
  Serial.print(str.c_str());
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Key definitions

static const uint8_t KEY_PWR = 0xFF; // This should be handled specially, but not yet sure how.
static const uint8_t KEY_NONE = 0; // Not really needed because it will never be detected.

static const uint8_t RIGHT_SHIFT = 0xe2;

static const uint8_t MINUS = 0x2d;
//static const uint8_t KEY_TAB = 0x09;
//static const uint8_t KEY_BACKSPACE = 0x08;
//static const uint8_t KEY_SPACE = 0x20;

/*

a:
KeyRelease event, serial 86, synthetic NO, window 0x3e00001,
    root 0x4dc, subw 0x0, time 8200295, (263,377), root:(412,664),
    state 0x0, keycode 38 (keysym 0x61, a), same_screen YES,
    XLookupString gives 1 bytes: (61) "a"
    XFilterEvent returns: False


Right Shift:
KeyRelease event, serial 87, synthetic NO, window 0x3e00001,
    root 0x4dc, subw 0x0, time 8253226, (2169,2196), root:(2318,2483),
    state 0x1, keycode 62 (keysym 0xffe2, Shift_R), same_screen YES,
    XLookupString gives 0 bytes: 
    XFilterEvent returns: False


0x2d
KeyPress event, serial 532, synthetic NO, window 0x3e00001,
    root 0x4dc, subw 0x0, time 62779919, (1879,1278), root:(1879,3769),
    state 0x0, keycode 40 (keysym 0x64, d), same_screen YES,
    XLookupString gives 1 bytes: (64) "d"
    XmbLookupString gives 1 bytes: (64) "d"
    XFilterEvent returns: False


*/

/////////////////////////////////////////////////////////////////////////////
// Pin definitions

static const uint8_t ROW_0 = 2;
static const uint8_t ROW_1 = 3;
static const uint8_t ROW_2 = 4;
static const uint8_t ROW_3 = 5;

static const uint8_t ROW_4 = 6;
static const uint8_t ROW_5 = 7;
static const uint8_t ROW_6 = 8;
static const uint8_t ROW_7 = 9;
/*
static const uint8_t _SRCLR = 21;
static const uint8_t _OE = 20;
static const uint8_t RCLK = 19;
static const uint8_t SER = 18;
static const uint8_t _DETECT = 14;
static const uint8_t SRCLK = 16;
*/

static const uint8_t _COL_0 = 10;
static const uint8_t _COL_1 = 16;
static const uint8_t _COL_2 = 14;
static const uint8_t _COL_3 = 18;
static const uint8_t _COL_4 = 19;
static const uint8_t _COL_5 = 20;
static const uint8_t _COL_6 = 21;

static const uint8_t _CLED = 15; // active low

static const uint8_t outputPins[] = {_CLED};
static const uint8_t inputPullupPins[] = {};

/////////////////////////////////////////////////////////////////////////////
// Layout

static const uint8_t columns[]={_COL_0, _COL_1, _COL_2, _COL_3, _COL_4, _COL_5, _COL_6};
// static const uint8_t columns[]={_COL_0, _COL_1, _COL_2, _COL_3, _COL_4};
static const uint8_t rows[]={ROW_0, ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7};

static const uint8_t MAX_COLS = sizeof(columns);
static const uint8_t MAX_ROWS = sizeof(rows);

static uint8_t keyMatrix[MAX_ROWS][MAX_COLS] = {
    {KEY_RIGHT_SHIFT, KEY_ESC, KEY_NONE, KEY_F11, KEY_NONE, MINUS, KEY_NONE},
    {KEY_PWR, KEY_F2, KEY_F3, 0xbe, KEY_F4, KEY_F5, KEY_F6},
    {KEY_LEFT_CTRL, 'x', 'c', 'z', 'v', 'b', 'n'},
    {KEY_LEFT_GUI, 's', 'd', 'a', 'f', 'g', 'h'},
    {KEY_LEFT_ALT, 'w', 'e', 'q', 'r', 't', 'y'},
    {KEY_CAPS_LOCK, '2', '3', '1', '4', '5', '6'},
    {KEY_LEFT_SHIFT, KEY_TAB, KEY_NONE, KEY_BACKSPACE, KEY_NONE, 0x27, KEY_SPACE},
    {KEY_NONE, 0x60, 0xa7, KEY_F12, KEY_NONE, 0x5b, KEY_NONE}
  };

// static const uint8_t keyMatrix[MAX_ROWS][MAX_COLS] = {
//     {KEY_RIGHT_SHIFT, KEY_ESC, KEY_NONE, KEY_F11, KEY_NONE},
//     {KEY_PWR, KEY_F2, KEY_F3, KEY_F1, KEY_F4},
//     {KEY_LEFT_CTRL, 'x', 'c', 'z', 'v'},
//     {KEY_LEFT_GUI, 's', 'd', 'a', 'f'}
//   };

static uint8_t previousMatrix[MAX_ROWS][MAX_COLS] = {{1}};
static uint8_t readMatrix[MAX_ROWS][MAX_COLS] = {{1}};

/////////////////////////////////////////////////////////////////////////////

PowerBook5300Keyboard::PowerBook5300Keyboard()
  {}

void PowerBook5300Keyboard::init() {
  ;
  for (uint8_t i=0; i<sizeof(columns); i++) {
    InputOutputTools::initOutput(columns[i]);
  }
  for (uint8_t i=0; i<sizeof(outputPins); i++) {
    InputOutputTools::initOutput(outputPins[i]);
  }
  for (uint8_t i=0; i<sizeof(rows); i++) {
    InputOutputTools::initInputPullup(rows[i]);
  }
  for (uint8_t i=0; i<sizeof(inputPullupPins); i++) {
    InputOutputTools::initInputPullup(inputPullupPins[i]);
  }
  BootKeyboard.begin();
}

void debugPrintMatrix(uint8_t matrix[MAX_ROWS][MAX_COLS]) {
  for (uint8_t row=0; row<MAX_ROWS; row++) {
    Serial.write("[ ");
    for (uint8_t column=0; column<MAX_COLS; column++) {
      String str = String(matrix[row][column])+String(" ");
      Serial.write(str.c_str());
    }
    Serial.write("]\n");
  }
  Serial.write("\n");
}

void debugPrintMatrices(uint8_t readMatrix[MAX_ROWS][MAX_COLS], uint8_t previousMatrix[MAX_ROWS][MAX_COLS]) {
  debugPrintMatrix(previousMatrix);
  debugPrintMatrix(readMatrix);
}

void PowerBook5300Keyboard::scanMatrix() {
  // Scan one column at a time by setting it's control line low
  // and reading the input pin for each row
  bool debugLog = false;
  for (uint8_t column=0; column<MAX_COLS; column++) {
    InputOutputTools::setLow(columns[column]);
    delayMicroseconds(15);
    for (uint8_t row=0; row<MAX_ROWS; row++) {
      previousMatrix[row][column] = readMatrix[row][column];
      readMatrix[row][column] = InputOutputTools::read(rows[row]);
      if (readMatrix[row][column] != previousMatrix[row][column]) {
        //debugLog=true;
        uint8_t keyCode = keyMatrix[row][column];
        if (readMatrix[row][column] == 1) {
          BootKeyboard.release(keyCode);
          debugPrint("Key released: ", row, column, keyCode);

        } else {
          BootKeyboard.press(keyCode);
          debugPrint("Key pressed: ", row, column, keyCode);
        }
      }
      if (debugLog) {
        String str = String("Row scan complete: "+String(row)+"\n");
        Serial.write(str.c_str());
      }
    }
    if (debugLog) {
      String str = String("Column scan complete: "+String(column)+"\n");
      Serial.write(str.c_str());
    }
    InputOutputTools::setHigh(columns[column]);
  }
  if (debugLog) {
    // debugPrintMatrix(readMatrix);
    // debugPrintMatrix(previousMatrix);
    String str = String("Matrix scan complete.\n");
    Serial.write(str.c_str());
  }
  debugLog=false;
}

void PowerBook5300Keyboard::interpretMatrix() {
  // Compare the read matrix to the previous matrix to determine what has changed.
  // If something has changed, send the corresponding key action and remember the
  // new value for the next scan.
  for (uint8_t column=0; column<MAX_COLS; column++) {
    for (uint8_t row=0; row<MAX_ROWS; row++) {
    }
  }
}

void PowerBook5300Keyboard::scan() {
  scanMatrix();
  interpretMatrix();
  // Check the status of the CapsLock LED
  if (BootKeyboard.getLeds() & LED_CAPS_LOCK) {
    // LED should be on
    InputOutputTools::setLow(_CLED);
  } else {
    // LED should be off
    InputOutputTools::setHigh(_CLED);
  }
}