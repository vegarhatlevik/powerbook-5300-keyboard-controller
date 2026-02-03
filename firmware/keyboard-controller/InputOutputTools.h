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
};