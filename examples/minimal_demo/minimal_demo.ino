// creates 'radio', 'display' and 'button' instances 
#include <heltec.h>

void setup() {
  heltec_setup();
  Serial.println("Serial works");
  // Display
  display.println("Display works");
  // Radio
  display.print("Radio ");
  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    display.println("works");
  } else {
    display.printf("fail, code: %i\n", state);
  }
  // Battery
  float vbat = VBat();
  display.printf("Vbat: %.2fV (%d%%)\n", vbat, BatteryPercent(vbat));
}

void loop() {
  heltec_loop();
  // Button
  if (button.isSingleClick()) {
    display.println("Button works");
    // LED
    for (int n = 0; n <= 100; n++) { led(n); delay(5); }
    for (int n = 100; n >= 0; n--) { led(n); delay(5); }
    display.println("LED works");
  }
}
