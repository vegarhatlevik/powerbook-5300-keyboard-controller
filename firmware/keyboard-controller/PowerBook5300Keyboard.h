class PowerBook5300Keyboard {
private:
  void setOutputPin(uint8_t pin, uint8_t value);
  void writeColumnBit(uint8_t column);
  void dectivateAllColumns();
  bool detectKey(uint8_t row, uint8_t column);
  void scanMatrix();
  void activateNextColumn();
public:
  PowerBook5300Keyboard();
  void init();
  void scan();
};