#ifndef heltec_h
#define heltec_h

#include <RadioLib.h>

#include "display/SSD1306Wire.h"
#include "display/OLEDDisplayUi.h"

#include "PinButton.h"

// Heltec's pins_arduino.h calls the pin DIO0, but what they hooked up 
// is actually DIO1 (!)
#define DIO1    DIO0

#define BUTTON  0

#define LED_PIN   35
#define LED_FREQ  5000
#define LED_CHAN  0
#define LED_RES   8

#define VEXT      36

#define VBAT_CTRL 37
#define VBAT_ADC  1

SX1262 radio = new Module(SS, DIO1, RST_LoRa, BUSY_LoRa);

SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED, RST_OLED, GEOMETRY_128_64);

PinButton button(BUTTON);

void heltec_setup() {
  Serial.begin(115200);
  display.init();
  display.flipScreenVertically();
}

void heltec_loop() {
  button.update();
}

void led(int percent) {
  if (percent > 0) {
    ledcSetup(LED_CHAN, LED_FREQ, LED_RES);
    ledcAttachPin(LED_PIN, LED_CHAN);
    ledcWrite(LED_CHAN, percent * 255 / 100);
  } else {
    pinMode(LED_PIN, INPUT);
  }
}

void Ve(bool state) {
  if (state) {
    pinMode(VEXT, OUTPUT);
    digitalWrite(VEXT, LOW);
  } else {
    // pulled up, no need to drive it
    pinMode(VEXT, INPUT);
  }
}

int VBat() {
  pinMode(VBAT_CTRL, OUTPUT);
  digitalWrite(VBAT_CTRL, LOW);
  delay(2);
  int vbat = analogRead(VBAT_ADC);
  // pulled up, no need to drive it
  pinMode(VBAT_CTRL, INPUT);
  return vbat;
}

#endif
