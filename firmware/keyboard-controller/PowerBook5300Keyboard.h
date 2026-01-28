class PowerBook5300Keyboard {
private:

  void scanMatrix();
  void interpretMatrix();
public:
  PowerBook5300Keyboard();
  void init();
  void scan();
};