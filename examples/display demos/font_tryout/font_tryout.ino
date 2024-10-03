/**
 * Using this sketch, you can try out the fonts. Simpky generate new font files
 * at https://oleddisplay.squix.ch/ , download them and add to your sketch with
 * Sketch / Add file in the Arduino IDE. Then include them and add their names
 * to loop(). You'll see the name of the font on the serial monitor as you click
 * through them with the button. A long press of the button turns the device
 * off.
*/

#define HELTEC_POWER_BUTTON
#include <heltec_unofficial.h>

#include "Dialog_plain_8.h"
#include "Dialog_plain_7.h"
#include "Dialog_plain_6.h"

#define DO_PRINT(a) doPrint(a, #a); 

void setup() {
  heltec_setup();
}

void loop() {
  
  // These three fonts come with the display driver
  DO_PRINT(ArialMT_Plain_10);
  DO_PRINT(ArialMT_Plain_16);
  DO_PRINT(ArialMT_Plain_24);

  // These three I had generated and added as files
  DO_PRINT(Dialog_plain_6);
  DO_PRINT(Dialog_plain_7);
  DO_PRINT(Dialog_plain_8);

}


void doPrint(const uint8_t* font, String fontname) {
  Serial.println(fontname);

  display.cls();
  display.setFont(font);

  display.println("Lorem ipsum dolor sit amet, consectetur");
  display.println("adipiscing elit, sed do eiusmod tempor");
  display.println("incididunt ut labore et dolore magna aliqua.");
  display.println("Ut enim ad minim veniam, quis nostrud exercitation");
  display.println("ullamco laboris nisi ut aliquip ex ea commodo");
  display.println("consequat. Duis aute irure dolor in reprehenderit");
  display.println("in voluptate velit esse cillum dolore eu fugiat");
  display.println("nulla pariatur. Excepteur sint occaecat cupidatat");
  display.println("non proident, sunt in culpa qui officia deserunt.");

  while(true) {
    heltec_loop();
    if (button.isSingleClick()) break;
  }
}

// The library comes with fonts as a const uint8_t array, this 
// https://oleddisplay.squix.ch/ site generates them as const char array.
// This code converts one in the other to make sure both work here.
void doPrint(const char* font, String fontname) {
  doPrint(static_cast<const uint8_t*>(reinterpret_cast<const void*>(font)), fontname);
}
