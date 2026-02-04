class InputOutputTools {
public:
  static void initOutput(uint8_t pin) {
    setHigh(pin);
  }

  static void initInputPullup(uint8_t pin) {
    pinMode(pin, INPUT_PULLUP);
  }

  static void initInput(uint8_t pin) {
    pinMode(pin, INPUT);
  }

  static void setLow(uint8_t pin) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);  
  }

  static void setHigh(uint8_t pin) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }

  static void setOutputPin(uint8_t pin, uint8_t value) {
    if (0 == value) {
      InputOutputTools::setLow(pin);
    } else {
      InputOutputTools::setHigh(pin);
    }
  }

  static uint8_t read(uint8_t pin) {
    pinMode(pin, INPUT_PULLUP);
    return digitalRead(pin); 
  }

  static void debugPrint(String s, uint8_t row, uint8_t column, uint8_t keyCode) {
#ifdef DEBUG
  char c = keyCode;
  String str = s + "row=" + row + ", column=" + column + ", 0x" + String(keyCode, HEX) + ", " + String(c) + "\n";
  Serial.print(str.c_str());
#endif
  }
};