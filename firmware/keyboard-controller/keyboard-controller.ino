//#include <HID.h>

#include "PowerBook5300Keyboard.h"

PowerBook5300Keyboard kbd;

void setup() {
  kbd.init();
  Keyboard.begin();
}

void loop() {
  kbd.scan();
}

