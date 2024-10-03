/**
 * Modified to "just work" with my library for the heltec esp32 lora v3 board
 * 
*/


#define HELTEC_POWER_BUTTON
#include <heltec_unofficial.h>

/**
   The MIT License (MIT)

   Copyright (c) 2018 by ThingPulse, Daniel Eichhorn
   Copyright (c) 2018 by Fabrice Weinberg

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

   ThingPulse invests considerable time and money to develop these open source libraries.
   Please support us by buying our products (and not the clones) from
   https://thingpulse.com

*/

// Adapted from Adafruit_SSD1306
void drawLines() {
  for (int16_t i = 0; i < display.getWidth(); i += 4) {
    display.drawLine(0, 0, i, display.getHeight() - 1);
    display.display();
    heltec_delay(10);
  }
  for (int16_t i = 0; i < display.getHeight(); i += 4) {
    display.drawLine(0, 0, display.getWidth() - 1, i);
    display.display();
    heltec_delay(10);
  }
  heltec_delay(250);

  display.clear();
  for (int16_t i = 0; i < display.getWidth(); i += 4) {
    display.drawLine(0, display.getHeight() - 1, i, 0);
    display.display();
    heltec_delay(10);
  }
  for (int16_t i = display.getHeight() - 1; i >= 0; i -= 4) {
    display.drawLine(0, display.getHeight() - 1, display.getWidth() - 1, i);
    display.display();
    heltec_delay(10);
  }
  heltec_delay(250);

  display.clear();
  for (int16_t i = display.getWidth() - 1; i >= 0; i -= 4) {
    display.drawLine(display.getWidth() - 1, display.getHeight() - 1, i, 0);
    display.display();
    heltec_delay(10);
  }
  for (int16_t i = display.getHeight() - 1; i >= 0; i -= 4) {
    display.drawLine(display.getWidth() - 1, display.getHeight() - 1, 0, i);
    display.display();
    heltec_delay(10);
  }
  heltec_delay(250);
  display.clear();
  for (int16_t i = 0; i < display.getHeight(); i += 4) {
    display.drawLine(display.getWidth() - 1, 0, 0, i);
    display.display();
    heltec_delay(10);
  }
  for (int16_t i = 0; i < display.getWidth(); i += 4) {
    display.drawLine(display.getWidth() - 1, 0, i, display.getHeight() - 1);
    display.display();
    heltec_delay(10);
  }
  heltec_delay(250);
}

// Adapted from Adafruit_SSD1306
void drawRect() {
  for (int16_t i = 0; i < display.getHeight() / 2; i += 2) {
    display.drawRect(i, i, display.getWidth() - 2 * i, display.getHeight() - 2 * i);
    display.display();
    heltec_delay(10);
  }
}

// Adapted from Adafruit_SSD1306
void fillRect() {
  uint8_t color = 1;
  for (int16_t i = 0; i < display.getHeight() / 2; i += 3) {
    display.setColor((color % 2 == 0) ? BLACK : WHITE); // alternate colors
    display.fillRect(i, i, display.getWidth() - i * 2, display.getHeight() - i * 2);
    display.display();
    heltec_delay(10);
    color++;
  }
  // Reset back to WHITE
  display.setColor(WHITE);
}

// Adapted from Adafruit_SSD1306
void drawCircle() {
  for (int16_t i = 0; i < display.getHeight(); i += 2) {
    display.drawCircle(display.getWidth() / 2, display.getHeight() / 2, i);
    display.display();
    heltec_delay(10);
  }
  heltec_delay(1000);
  display.clear();

  // This will draw the part of the circel in quadrant 1
  // Quadrants are numberd like this:
  //   0010 | 0001
  //  ------|-----
  //   0100 | 1000
  //
  display.drawCircleQuads(display.getWidth() / 2, display.getHeight() / 2, display.getHeight() / 4, 0b00000001);
  display.display();
  heltec_delay(200);
  display.drawCircleQuads(display.getWidth() / 2, display.getHeight() / 2, display.getHeight() / 4, 0b00000011);
  display.display();
  heltec_delay(200);
  display.drawCircleQuads(display.getWidth() / 2, display.getHeight() / 2, display.getHeight() / 4, 0b00000111);
  display.display();
  heltec_delay(200);
  display.drawCircleQuads(display.getWidth() / 2, display.getHeight() / 2, display.getHeight() / 4, 0b00001111);
  display.display();
}

void printBuffer() {
  // Some test data
  const char* test[] = {
    "Hello World!",
    "This goes to" ,
    "show that",
    "you can",
    "print to the",
    "tiny display.",
    "As you can",
    "see scrolling",
    "works just",
    "fine. Have a",
    "nice day !"
  };
  display.clear();
  for (uint8_t i = 0; i < 11; i++) {
    // Print to the screen
    display.println(test[i]);
    heltec_delay(750);
  }
}

void setup() {
  heltec_setup();

  drawLines();
  heltec_delay(1000);
  display.clear();

  drawRect();
  heltec_delay(1000);
  display.clear();

  fillRect();
  heltec_delay(1000);
  display.clear();

  drawCircle();
  heltec_delay(1000);
  display.clear();

  printBuffer();
  heltec_delay(1000);
  display.clear();
}

void loop() {
  heltec_loop();
}
