/*
 *  See https://github.com/ropg/HotButton for documentation and for reporting
 *  any issues.
*/


#define MAX_PRESSES 4
#define DEBOUNCE_TIME 20
#define EVENT_TIMEOUT 250

#define BETWEEN(a, b)   ((((a / 10) & 0xFF) << 8) | ((b / 10) & 0xFF))
#define OVER(a)         ((((a / 10) & 0xFF) << 8) | 0xFF)
#define UNDER(b)        ((b / 10) & 0xFF)

#define DIT             UNDER(200)
#define SHORT           DIT
#define DA              OVER(200)
#define LONG            DA

#define SINGLECLICK     DIT
#define DOUBLECLICK     DIT, DIT
#define TRIPLECLICK     DIT, DIT, DIT
#define QUADRUPLECLICK  DIT, DIT, DIT, DIT

class HotButton {
  public:
    HotButton(int _pin, bool _pullup = false, bool _active = LOW) {
      pin = _pin;
      pullup = _pullup;
      active = _active;
    }
    void update() {
      pinMode(pin, pullup ? INPUT_PULLUP : INPUT);
      // deaf for debounceTime ms after press or release
      if (millis() - lastChange < DEBOUNCE_TIME) return;
      // if released for EVENT_TIMEOUT, go idle
      if (!curState && millis() - lastChange > EVENT_TIMEOUT && !idle) idle = true;
      // inverting and XORing active: state true when button pressed
      bool state = !digitalRead(pin) ^ active;
      // same as last time, nothing to do
      if (state == curState) return;
      // state has changed
      if (state) {
        // button pressed, forget previous event clicks if idle in-between
        if (idle) pressCount = 0;
      } else if (pressCount < MAX_PRESSES) {
        // button released, store button press in ms / 10
        presses[pressCount++] = min(255, (int)((millis() - lastChange) / 10));
        lastPressedFor = 0;
      }
      idle = false;
      curState = state;
      lastChange = millis(); 
    }
    bool event(uint16_t click1) {
      if (
        idle && pressCount == 1 &&
        isBetween(click1, presses[0] * 10)
      ) {
        pressCount = 0;
        return true;
      }
      return false;
    }
    bool event(uint16_t click1, uint16_t click2) {
      if (
        idle && pressCount == 2 &&
        isBetween(click1, presses[0] * 10) && 
        isBetween(click2, presses[1] * 10)
      ) {
        pressCount = 0;
        return true;          
      }
      return false;
    }
    bool event(uint16_t click1, uint16_t click2, uint16_t click3) {
      if (
        idle && pressCount == 3 &&
        isBetween(click1, presses[0] * 10) && 
        isBetween(click2, presses[1] * 10) &&
        isBetween(click3, presses[2] * 10)
      ) {
        pressCount = 0;
        return true;          
      }
      return false;
    }
    bool event(uint16_t click1, uint16_t click2, uint16_t click3, uint16_t click4) {
      if (
        idle && pressCount == 4 &&
        isBetween(click1, presses[0] * 10) && 
        isBetween(click2, presses[1] * 10) &&
        isBetween(click3, presses[2] * 10) &&
        isBetween(click4, presses[3] * 10)
      ) {
        pressCount = 0;
        return true;          
      }
      return false;
    }
    bool isSingleClick() {
      return event(SINGLECLICK);
    }
    bool isDoubleClick() {
      return event(DOUBLECLICK);
    }
    bool isTripleClick() {
      return event(TRIPLECLICK);
    }
    bool isQuadrupleClick() {
      return event(QUADRUPLECLICK);
    }
    bool pressedFor(int time) {
      if (curState && millis() - lastChange > time && time > lastPressedFor) {
        lastPressedFor = time;
        return true;
      }
      return false;
    }
    bool pressed() {
      return pressedFor(1);
    }
    bool pressedNow() {
      pinMode(pin, pullup ? INPUT_PULLUP : INPUT);
      return digitalRead(pin) == active;
    }
    void waitForPress() {
      pinMode(pin, pullup ? INPUT_PULLUP : INPUT);
      while (digitalRead(pin) != active) delay (10);
      delay(20);
    }
    void waitForRelease() {
      pinMode(pin, pullup ? INPUT_PULLUP : INPUT);
      while (digitalRead(pin) == active) delay (10);
      delay(20);
    }

  private:
    static bool isBetween(const uint16_t betweenValue, int time) {
      int from = (betweenValue >> 8) * 10;
      int to = (betweenValue & 0xFF) * 10;
      return time > from && time <= to;
    }
    uint8_t pin;
    bool pullup;
    bool active;
    bool curState;
    bool idle;
    uint8_t presses[MAX_PRESSES];
    uint8_t pressCount = 0;
    uint32_t lastChange = 0;
    uint16_t lastPressedFor = 0;
};
