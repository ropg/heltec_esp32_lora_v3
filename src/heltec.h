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

// Don't you just hate it when battery percentages are wrong?
// 
// I measured the actual voltage drop on a LiPo battery and these 
// are the average voltages, expressed in 1/256'th steps between 
// min_voltage and max_voltage for each 1/100 of the time it took to
// discharge the battery. The code for a telnet server that outputs
// battery voltage as CSV data is in examples, and a python script
// that outputs the constants below is in src/tools.
const float min_voltage = 3.04;
const float max_voltage = 4.26;
const uint8_t scaled_voltage[100] = {
    254, 242, 230, 227, 223, 219, 215, 213, 210, 207,
    206, 202, 202, 200, 200, 199, 198, 198, 196, 196,
    195, 195, 194, 192, 191, 188, 187, 185, 185, 185,
    183, 182, 180, 179, 178, 175, 175, 174, 172, 171,
    170, 169, 168, 166, 166, 165, 165, 164, 161, 161,
    159, 158, 158, 157, 156, 155, 151, 148, 147, 145,
    143, 142, 140, 140, 136, 132, 130, 130, 129, 126,
    125, 124, 121, 120, 118, 116, 115, 114, 112, 112,
    110, 110, 108, 106, 106, 104, 102, 101, 99, 97,
    94, 90, 81, 80, 76, 73, 66, 52, 32, 7,
};

class PrintSplitter : public Print {
  public:
    PrintSplitter(Print &_a, Print &_b) : a(_a), b(_b) {}
    size_t write(uint8_t c) {
      a.write(c);
      return b.write(c);
    }
    size_t write(const char* str) {
      a.write(str);
      return b.write(str);
    }
  private:
    Print &a;
    Print &b;
};

SX1262 radio = new Module(SS, DIO1, RST_LoRa, BUSY_LoRa);

SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED, RST_OLED, GEOMETRY_128_64);

PrintSplitter both(Serial, display);

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
    ledcDetachPin(LED_PIN);
    pinMode(VEXT, INPUT);
  }
}

float VBat() {
  pinMode(VBAT_CTRL, OUTPUT);
  digitalWrite(VBAT_CTRL, LOW);
  delay(5);
  float vbat = analogRead(VBAT_ADC) / 238.7;
  // pulled up, no need to drive it
  pinMode(VBAT_CTRL, INPUT);
  return vbat;
}

int BatteryPercent(float vbat = -1) {
  if (vbat == -1) {
    vbat = VBat();
  }
  for (int n = 0; n < sizeof(scaled_voltage); n++) {
    float step = (max_voltage - min_voltage) / 256;
    if (vbat > min_voltage + (step * scaled_voltage[n])) {
      return 100 - n;
    }
  }
  return 0;
}

#endif
