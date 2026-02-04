#include "PowerBook5300Keyboard.h"
#define DEBUG
#include "InputOutputTools.h"

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

static const uint8_t SRCLR = 21;
static const uint8_t OE = 20;
static const uint8_t SER = 18;
static const uint8_t SRCLK = 16;

static const uint8_t CLED = 15; // active low

static const uint8_t outputPins[] = {CLED, SRCLR, OE, SER, SRCLK};
static const uint8_t inputPullupPins[] = {};

/////////////////////////////////////////////////////////////////////////////
// Layout

static const uint8_t rows[]={ROW_0, ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7};
static const uint8_t MAX_COLS = 11;
static const uint8_t MAX_ROWS = sizeof(rows);

#define KEY_NONE KEY_RESERVED

static KeyboardKeycode keyMatrix[MAX_ROWS][MAX_COLS] = {
    {KEY_RIGHT_SHIFT,     KEY_ESC,        KEY_NONE,     KEY_F11,        KEY_NONE, KEY_MINUS,      KEY_NONE,   KEY_NONE, KEY_RETURN,       KEYPAD_ENTER, KEY_BACKSLASH},
    {HID_KEYBOARD_POWER,  KEY_F2,         KEY_F3,       KEY_F1,         KEY_F4,   KEY_F5,         KEY_F6,     KEY_F7,   KEY_F10,          KEY_F8,       KEY_F9},
    {KEY_LEFT_CTRL,       KEY_X,          KEY_C,        KEY_Z,          KEY_V,    KEY_B,          KEY_N,      KEY_M,    KEY_SLASH,        KEY_COMMA,    KEY_PERIOD},
    {KEY_LEFT_GUI,        KEY_S,          KEY_D,        KEY_A,          KEY_F,    KEY_G,          KEY_H,      KEY_J,    KEY_SEMICOLON,    KEY_K,        KEY_L},
    {KEY_LEFT_ALT,        KEY_W,          KEY_E,        KEY_Q,          KEY_R,    KEY_T,          KEY_Y,      KEY_U,    KEY_P,            KEY_I,        KEY_O},
    {KEY_CAPS_LOCK,       KEY_2,          KEY_3,        KEY_1,          KEY_4,    KEY_5,          KEY_6,      KEY_7,    KEY_0,            KEY_8,        KEY_9},
    {KEY_LEFT_SHIFT,      KEY_TAB,        KEY_NONE,     KEY_BACKSPACE,  KEY_NONE, KEY_QUOTE,      KEY_SPACE,  KEY_NONE, KEY_RIGHT_BRACE,  KEY_LEFT,     KEY_DOWN},
    {KEY_NONE,            KEY_NON_US,     KEY_TILDE,    KEY_F12,        KEY_NONE, KEY_LEFT_BRACE, KEY_NONE,   KEY_NONE, KEY_EQUAL,        KEY_RIGHT,    KEY_UP}
  };

static uint8_t previousMatrix[MAX_ROWS][MAX_COLS] = {{1}};
static uint8_t readMatrix[MAX_ROWS][MAX_COLS] = {{1}};

/////////////////////////////////////////////////////////////////////////////

PowerBook5300Keyboard::PowerBook5300Keyboard()
  {}

void PowerBook5300Keyboard::init() {
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

///////////////////////////////////////////////////////////////////////////////
//
// detectKey
//
// Detects if the key at the specified position (row, column) is currently
// pressed and returns true if that represents a change since the last scan.
// 
bool PowerBook5300Keyboard::detectKey(uint8_t row, uint8_t column) {
  previousMatrix[row][column] = readMatrix[row][column];
  readMatrix[row][column] = InputOutputTools::read(rows[row]);
  return (readMatrix[row][column] != previousMatrix[row][column]);
}

///////////////////////////////////////////////////////////////////////////////
//
// processKey
//
// Checks if the key at the specified position (row, column) has changed state
// since the last scan, and adds or removes it from the report accordingly.
// 
void PowerBook5300Keyboard::processKey(uint8_t row, uint8_t column) {
  if (readMatrix[row][column] != previousMatrix[row][column]) {
    KeyboardKeycode keyCode = keyMatrix[row][column];
    if (readMatrix[row][column] == 1) {
      BootKeyboard.remove(keyCode);
      InputOutputTools::debugPrint("Key released: ", row, column, keyCode);
    } else {
      BootKeyboard.add(keyCode);
      InputOutputTools::debugPrint("Key pressed: ", row, column, keyCode);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
// writeColumnBit
//
// Writes one bit to the column shift register.
//
void PowerBook5300Keyboard::writeColumnBit(uint8_t bit) {
  InputOutputTools::setHigh(OE);
  InputOutputTools::setOutputPin(SER, bit);
  InputOutputTools::setLow(SRCLK);
  delayMicroseconds(1);
  InputOutputTools::setHigh(SRCLK);
  delayMicroseconds(1);
}

///////////////////////////////////////////////////////////////////////////////
//
// deactivateAllColumns
//
// Deactivates all columns by disabling the shift register output and setting
// us up for another column scan.
//
void PowerBook5300Keyboard::deactivateAllColumns() {
  InputOutputTools::setHigh(OE);
  delayMicroseconds(1);
  for (int i=0; i<MAX_COLS; i++) {
    writeColumnBit(HIGH);
    delayMicroseconds(1);
  }
  // Load a 0 into first position of shift register.
  // This is needed because the output register is one clock cycle
  // behind the shift register, so by writing it in now, we have set
  // it up for the next scan cycle.
  writeColumnBit(LOW);
  delayMicroseconds(1);
}

///////////////////////////////////////////////////////////////////////////////
//
// activateNextColumn
//
// Activates the next column by shifting the register by one and enabling
// the shift register output.
//
void PowerBook5300Keyboard::activateNextColumn() {
  writeColumnBit(HIGH);
  InputOutputTools::setLow(OE);
  delayMicroseconds(1);
}

///////////////////////////////////////////////////////////////////////////////
//
// scanMatrix
//
// Scans the entire keyboard matrix to determine which keys are currently
// pressed.
//
void PowerBook5300Keyboard::scanMatrix() {
  bool debugLog = false;
  deactivateAllColumns();
  for (uint8_t column=0; column<MAX_COLS; column++) {
    activateNextColumn();
    for (uint8_t row=0; row<MAX_ROWS; row++) {
      debugLog |= detectKey(row, column);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
// processMatrix
//
// Add all changed key presses (press or release) to the report and send it.
//
void PowerBook5300Keyboard::processMatrix() {
  for (uint8_t column=0; column<MAX_COLS; column++) {
    for (uint8_t row=0; row<MAX_ROWS; row++) {
      processKey(row, column);
    }
    BootKeyboard.send();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// scan
//
// Perform a full scan of the keyboard, send the report to the host and act on
// any status changes for keyboard LEDs.
//
void PowerBook5300Keyboard::scan() {
  scanMatrix();
  processMatrix();
  // Check the status of the CapsLock LED
  if (BootKeyboard.getLeds() & LED_CAPS_LOCK) {
    // LED should be on
    InputOutputTools::setLow(CLED);
  } else {
    // LED should be off
    InputOutputTools::setHigh(CLED);
  }
}