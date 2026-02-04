// I don't know why I need to define USBCON. My board is definitely supported.
// I have a Sparkfun Pro Micro, which is Leonardo compatible. Yet, I get an error
// that my board is not supported if I don't define USBCON. I even tried with an
// actual Leonrdo board, and I have the same problem. So, I will leave this define
// here until I understand what is going wrong. With the define, it works fine.
#define USBCON
// Select which layout we're implementing
#define HID_CUSTOM_LAYOUT
//#define LAYOUT_UNITED_KINGDOM
#define LAYOUT_US_ENGLISH
//#define LAYOUT_GERMAN
// #define LAYOUT_GERMAN_MAC
#include <HID-Project.h>

class PowerBook5300Keyboard {
private:
  void setOutputPin(uint8_t pin, uint8_t value);
  void writeColumnBit(uint8_t column);
  void activateNextColumn();
  void deactivateAllColumns();
  bool detectKey(uint8_t row, uint8_t column);
  void processKey(uint8_t row, uint8_t column);
  void scanMatrix();
  void processMatrix();
public:
  PowerBook5300Keyboard();
  void init();
  void scan();
};