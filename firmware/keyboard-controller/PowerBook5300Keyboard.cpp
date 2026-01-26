#include "PowerBook5300Keyboard.h"
#include "InputOutputTools.h"

#define DEBUG

static void debugPrint(String s, uint8_t row, uint8_t column, uint8_t keyCode) {
#ifdef DEBUG
  char c = keyCode;
  String str = s + "row=" + row + ", column=" + column + ", " + String(keyCode) + ", " + String(c) + "\n";
  Serial.print(str.c_str());
#endif
}

void PowerBook5300Keyboard::init() {
  for (uint8_t i=0; i<sizeof(outputPins); i++) {
    InputOutputTools::initOutput(outputPins[i]);
  }
  for (uint8_t i=0; i<sizeof(inputPullupPins); i++) {
    InputOutputTools::initInputPullup(inputPullupPins[i]);
  }
}

void PowerBook5300Keyboard::scan() {
  // Scan one column at a time by setting it's control line low
  // and reading the input pin for each row
  for (uint8_t column=0; column<MAX_COLS; column++) {
    InputOutputTools::setLow(columns[column]);
    delayMicroseconds(50);
    for (uint8_t row=0; row<MAX_ROWS; row++) {
      readMatrix[row][column] = InputOutputTools::read(rows[row]);
      // if (readMatrix[i][j] == 0) {
      //   Keyboard.write(keyMatrix[i][j]);
      // }
    }
    InputOutputTools::setHigh(columns[column]);
  }
  // Compare the read matrix to the previous matrix to determine what has changed.
  // If something has changed, send the corresponding key action and remember the
  // new value for the next scan.
  for (uint8_t column=0; column<MAX_COLS; column++) {
    for (uint8_t row=0; row<MAX_ROWS; row++) {
      if (readMatrix[row][column] != previousMatrix[row][column]) {
        uint8_t keyCode = keyMatrix[row][column];
        if (readMatrix[row][column] == 1) {
          Keyboard.release(keyCode);
          debugPrint("Key released: ", row, column, keyCode);

        } else {
          Keyboard.press(keyCode);
          debugPrint("Key pressed: ", row, column, keyCode);
        }
        previousMatrix[row][column] = readMatrix[row][column];
      }
    }
  }
}