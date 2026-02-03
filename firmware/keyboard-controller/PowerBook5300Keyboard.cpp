// I don't know why I need to define USBCON. My board is definitely supported.
// I have a Sparkfun Pro Micro, which is Leonardo compatible. Yet, I get an error
// that my board is not supported if I don't define USBCON. I even tried with an
// actual Leonrdo board, and I have the same problem. So, I will leave this define
// here until I understand what is going wrong. With the define, it works fine.
#define USBCON
// Select which layout we're implementing
#define HID_CUSTOM_LAYOUT
//#define LAYOUT_UNITED_KINGDOM
//#define LAYOUT_US_ENGLISH
#define LAYOUT_GERMAN
// #define LAYOUT_GERMAN_MAC
#include <HID-Project.h>
//#include <HID-Project/SingleReport/BootKeyboard.h>

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

// static const uint8_t COL_A = 10;
// static const uint8_t COL_B = 16;
// static const uint8_t COL_C = 14;
// static const uint8_t COL_D = 18;
// static const uint8_t COL_E = 19;
// static const uint8_t COL_F = 20;
// static const uint8_t COL_OE = 21;

// Output pins 14, 16, 18 and 19 for serial to parallel converter for columns

static const uint8_t SRCLR = 21;
static const uint8_t OE = 20;
static const uint8_t SER = 18;
static const uint8_t SRCLK = 16;

static const uint8_t CLED = 15; // active low

//static const uint8_t outputPins[] = {CLED, COL_A, COL_B, COL_C, COL_D, COL_E, COL_F, COL_OE};
static const uint8_t outputPins[] = {CLED, SRCLR, OE, SER, SRCLK};
static const uint8_t inputPullupPins[] = {};

/////////////////////////////////////////////////////////////////////////////
// Layout

//static const uint8_t columns[]={COL_A, COL_B, COL_C, COL_D, COL_E, COL_F};
//static const uint8_t columns[]={_COL_0, _COL_1, _COL_2, _COL_3, _COL_4, _COL_5, _COL_6, _COL_7, _COL_8, _COL_9, _COL_10};
// static const uint8_t columns[]={_COL_0, _COL_1, _COL_2, _COL_3, _COL_4};
static const uint8_t rows[]={ROW_0, ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7};

//static const uint8_t MAX_COLS = sizeof(columns);
static const uint8_t MAX_COLS = 11;
static const uint8_t MAX_ROWS = sizeof(rows);

/////////////////////////////////////////////////////////////////////////////
// Key definitions

static const uint8_t KEY_PWR = 0xFF; // This should be handled specially, but not yet sure how.
static const uint8_t KEY_NONE = 0; // Not really needed because it will never be detected.

static const uint8_t RIGHT_SHIFT = 0xe2;
static const uint8_t LEFT_SHIFT = 0xe1;

static const uint8_t TAB = 0x09;
static const uint8_t BACKSPACE = 0x08;
static const uint8_t SPACE = 0x20;

static const uint8_t  LEFT_ALT = 0xe9;
#define COMMA KEY_NONE
#define PERIOD KEY_NONE
#define MINUS KEY_NONE
#define PLUS KEY_NONE
#define GRAVE KEY_NONE

#ifdef LAYOUT_GERMAN
static uint8_t keyMatrix[MAX_ROWS][MAX_COLS] = {
    {RIGHT_SHIFT, KEY_ESC, KEY_NONE, KEY_F11, KEY_NONE, KEY_DE_SZ, KEY_NONE, KEY_NONE, KEY_RETURN, KEYPAD_ENTER, KEY_DE_HASHTAG2},
    {HID_KEYBOARD_POWER, KEY_F2, KEY_F3, KEY_F1, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F10, KEY_F8, KEY_F9},
    {KEY_LEFT_CTRL, 'x', 'c', 'z', 'v', 'b', 'n', 'm', MINUS, COMMA, PERIOD},
    {KEY_LEFT_GUI, 's', 'd', 'a', 'f', 'g', 'h', 'j', KEY_DE_OE, 'k', 'l'},
    {LEFT_ALT, 'w', 'e', 'q', 'r', 't', 'y', 'u', 'p', 'i', 'o'},
    {KEY_CAPS_LOCK, '2', '3', '1', '4', '5', '6', '7', '8', '9'},
    {LEFT_SHIFT, TAB, KEY_NONE, BACKSPACE, KEY_NONE, KEY_DE_AE, SPACE, KEY_NONE, PLUS, KEY_LEFT, KEY_DOWN},
    {KEY_NONE, KEY_DE_SMALLER, KEY_DE_DACH, KEY_F12, KEY_NONE, KEY_DE_UE, KEY_NONE, KEY_NONE, GRAVE, KEY_RIGHT, KEY_UP}
  };
#else
static uint8_t keyMatrix[MAX_ROWS][MAX_COLS] = {
    {RIGHT_SHIFT, KEY_ESC, KEY_NONE, KEY_F11, KEY_NONE, KEY_DE_SZ, KEY_NONE},
    {KEY_PWR, KEY_F2, KEY_F3, 0xbe, KEY_F4, KEY_F5, KEY_F6},
    {KEY_LEFT_CTRL, 'x', 'c', 'z', 'v', 'b', 'n'},
    {KEY_LEFT_GUI, 's', 'd', 'a', 'f', 'g', 'h'},
    {LEFT_ALT, 'w', 'e', 'q', 'r', 't', 'y'},
    {KEY_CAPS_LOCK, '2', '3', '1', '4', '5', '6'},
    {LEFT_SHIFT, TAB, KEY_NONE, BACKSPACE, KEY_NONE, 0x27, SPACE},
    {KEY_NONE, 0x3c, 0x60, KEY_F12, KEY_NONE, 0x5b, KEY_NONE}
  };
#endif

static uint8_t previousMatrix[MAX_ROWS][MAX_COLS] = {{1}};
static uint8_t readMatrix[MAX_ROWS][MAX_COLS] = {{1}};

/////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////

PowerBook5300Keyboard::PowerBook5300Keyboard()
  {}

void PowerBook5300Keyboard::init() {
  ;
  // for (uint8_t i=0; i<sizeof(columns); i++) {
  //   InputOutputTools::initOutput(columns[i]);
  // }
  for (uint8_t i=0; i<sizeof(outputPins); i++) {
    InputOutputTools::initOutput(outputPins[i]);
  }
  for (uint8_t i=0; i<sizeof(rows); i++) {
    InputOutputTools::initInputPullup(rows[i]);
  }
  for (uint8_t i=0; i<sizeof(inputPullupPins); i++) {
    InputOutputTools::initInputPullup(inputPullupPins[i]);
  }
  InputOutputTools::setHigh(OE);
  BootKeyboard.begin();
}

bool PowerBook5300Keyboard::detectKey(uint8_t row, uint8_t column) {
  bool rval = false;
  previousMatrix[row][column] = readMatrix[row][column];
  readMatrix[row][column] = InputOutputTools::read(rows[row]);
  if (readMatrix[row][column] != previousMatrix[row][column]) {
    rval = true;
    uint8_t keyCode = keyMatrix[row][column];
    if (readMatrix[row][column] == 1) {
      BootKeyboard.release(keyCode);
      debugPrint("Key released: ", row, column, keyCode);
    } else {
      BootKeyboard.press(keyCode);
      debugPrint("Key pressed: ", row, column, keyCode);
    }
  }
  return rval;
}

void PowerBook5300Keyboard::writeColumnBit(uint8_t bit) {
  InputOutputTools::setHigh(OE);
  InputOutputTools::setOutputPin(SER, bit);
  InputOutputTools::setLow(SRCLK);
  delayMicroseconds(5);
  InputOutputTools::setHigh(SRCLK);
  delayMicroseconds(5);
}

void PowerBook5300Keyboard::dectivateAllColumns() {
  InputOutputTools::setHigh(OE);
  delayMicroseconds(1);
  for (int i=0; i<8; i++) {
    writeColumnBit(HIGH);
    delayMicroseconds(1);
  }
  // Load a 0 into first position of shift register.
  // This is needed because othe utput register is one clock cycle
  // behind the shift register
  writeColumnBit(LOW);
  delayMicroseconds(1);
}

void PowerBook5300Keyboard::activateNextColumn() {
  writeColumnBit(HIGH);
  InputOutputTools::setLow(OE);
  delayMicroseconds(1);
}

void PowerBook5300Keyboard::scanMatrix() {
  bool debugLog = false;
  // First clear shift register
  dectivateAllColumns();
  // Scan one column at a time by setting its control line low
  // and reading the input pin for each row
  for (uint8_t column=0; column<MAX_COLS; column++) {
    activateNextColumn();
    for (uint8_t row=0; row<MAX_ROWS; row++) {
      debugLog = detectKey(row, column);
    }
  }
  dectivateAllColumns();
  if (debugLog) {
    debugPrintMatrix(readMatrix);
     // debugPrintMatrix(previousMatrix);
    String str = String("Matrix scan complete.\n");
    Serial.write(str.c_str());
    debugLog = false;
  }
}

void PowerBook5300Keyboard::scan() {
  scanMatrix();
  // Check the status of the CapsLock LED
  if (BootKeyboard.getLeds() & LED_CAPS_LOCK) {
    // LED should be on
    InputOutputTools::setLow(CLED);
  } else {
    // LED should be off
    InputOutputTools::setHigh(CLED);
  }
}