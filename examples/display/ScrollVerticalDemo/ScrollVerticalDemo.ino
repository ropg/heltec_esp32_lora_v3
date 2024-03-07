/**
 * Modified to "just work" with my library for the heltec esp32 lora v3 board
 * 
*/

#include <heltec.h>

/**
   The MIT License (MIT)

   Copyright (c) 2022 by Stefan Seyfried

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

// UTF-8 sprinkled within, because it tests special conditions in the char-counting code
const String loremipsum = "Lorem ipsum dolor sit ämet, "
  "consetetur sadipscing elitr, sed diam nonümy eirmöd "
  "tempor invidunt ut labore et dolore mägnä aliquyam erat, "
  "sed diam voluptua. At vero eos et accusam et justo duo "
  "dolores et ea rebum. Stet clita kasd gubergren, no sea "
  "takimata sanctus est Lorem ipsum dolor sit amet. "
  "äöü-ÄÖÜ/ß€é/çØ.";

void setup() {
  display.init();
  display.setContrast(255);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.display();
}

void loop() {
  static uint16_t start_at = 0;
  display.clear();
  uint16_t firstline = display.drawStringMaxWidth(0, 0, 128, loremipsum.substring(start_at));
  display.display();
  if (firstline != 0) {
    start_at += firstline;
  } else {
    start_at = 0;
    delay(1000); // additional pause before going back to start
  }
  delay(1000);
}
