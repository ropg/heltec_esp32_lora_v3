/**
 * This will allow you to measure battery current in deep sleep. If you reset it
 * will say "deep sleep testing tool", and then if you single-click the user
 * button it will select the user button as a wakeup source and if you
 * double-click it will add the clock as a wakeup source. Then press the button
 * for longer and it will go to deep sleep. So if you just reset and then
 * long-press the user button, there will be no wakeup sources selected and
 * you'll have to reset to wake up again.
 *
 * For measuring current, make sure you measure in series with the battery while
 * USB-C is disconnected.
*/

#include <heltec_unofficial.h>

bool buttonWake = false;
bool clockWake  = false;
uint32_t lastPress = 0;

void setup() {
  heltec_setup();
  // If the clock woke us up, go right back to sleep
  if (heltec_wakeup_was_timer()) {
    heltec_deep_sleep(3600 * 24);
  }
  both.println("Deep Sleep");
  both.println("Testing Tool");
}

void loop() {
  heltec_loop();

  if (button.pressedFor(1000) || millis() > 60000) {
    if (!buttonWake && !clockWake) {
      both.println("Reset only");
      delay(1000);
    }
    display.cls();

    // Wait for button release, or it will wake us up again
    while (digitalRead(BUTTON) == LOW) {}
    delay (20);

    // Wake up on button press if selected
    if (buttonWake) {
      esp_sleep_enable_ext0_wakeup(BUTTON, LOW);
    }

    // Wakes up once a day if selected
    // (but then fall right back to sleep)
    heltec_deep_sleep(clockWake ? 3600 * 24 : 0);
  }

  if (button.isSingleClick()) {
    if (!buttonWake) both.println("Button wakes");
    buttonWake = true;
  }

  if (button.isDoubleClick()) {
    if (!clockWake) both.println("Clock wakes");
    clockWake = true; 
  }

}
