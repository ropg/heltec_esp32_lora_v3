/**
 * @file heltec.h
 * @brief Header file for the Heltec library.
 * 
 * This file contains the definitions and declarations for the Heltec library.
 * The library provides functions for controlling the Heltec ESP32 LoRa V3
 * board, including LED brightness control, voltage measurement, deep sleep
 * mode, and more.
 */

#ifndef heltec_h
#define heltec_h

#ifndef HELTEC_NO_RADIOLIB
  #include <RadioLib.h>
#endif

#include "display/SSD1306Wire.h"
#include "display/OLEDDisplayUi.h"

#include "PinButton.h"

// Heltec's pins_arduino.h calls the pin DIO0, but what they hooked up 
// is actually DIO1 (!)
#define DIO1      DIO0

#define BUTTON    GPIO_NUM_0

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

/**
 * @class PrintSplitter
 * @brief A class that splits the output of the Print class to two different Print objects.
 * 
 * The PrintSplitter class is used to split the output of the Print class to two different Print objects.
 * It overrides the write() function to write the data to both Print objects.
 */
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

#ifndef HELTEC_NO_RADIOLIB
  SX1262 radio = new Module(SS, DIO1, RST_LoRa, BUSY_LoRa);
#endif

SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED, RST_OLED, GEOMETRY_128_64);

PrintSplitter both(Serial, display);

PinButton button(BUTTON);

/**
 * @brief Controls the LED brightness based on the given percentage.
 * 
 * This function sets up the LED channel, frequency, and resolution, and then adjusts the LED brightness
 * based on the given percentage. If the percentage is 0 or less, the LED pin is set as an input pin.
 * 
 * @param percent The brightness percentage of the LED (0-100).
 */
void heltec_led(int percent) {
  if (percent > 0) {
    ledcSetup(LED_CHAN, LED_FREQ, LED_RES);
    ledcAttachPin(LED_PIN, LED_CHAN);
    ledcWrite(LED_CHAN, percent * 255 / 100);
  } else {
    pinMode(LED_PIN, INPUT);
  }
}

/**
 * @brief Controls the VEXT pin to enable or disable external power.
 * 
 * This function sets the VEXT pin as an output pin and sets its state based on the given parameter.
 * If the state is true, the VEXT pin is set to LOW to enable external power. If the state is false,
 * the VEXT pin is set to INPUT to disable external power.
 * 
 * @param state The state of the VEXT pin (true = enable, false = disable).
 */
void heltec_ve(bool state) {
  if (state) {
    pinMode(VEXT, OUTPUT);
    digitalWrite(VEXT, LOW);
  } else {
    // pulled up, no need to drive it
    ledcDetachPin(LED_PIN);
    pinMode(VEXT, INPUT);
  }
}

/**
 * @brief Measures the battery voltage.
 * 
 * This function measures the battery voltage by controlling the VBAT_CTRL pin and reading the analog value
 * from the VBAT_ADC pin. The measured voltage is then converted to a float value and returned.
 * 
 * @return The battery voltage in volts.
 */
float heltec_vbat() {
  pinMode(VBAT_CTRL, OUTPUT);
  digitalWrite(VBAT_CTRL, LOW);
  delay(5);
  float vbat = analogRead(VBAT_ADC) / 238.7;
  // pulled up, no need to drive it
  pinMode(VBAT_CTRL, INPUT);
  return vbat;
}

/**
 * @brief Puts the device into deep sleep mode.
 * 
 * This function prepares the device for deep sleep mode by disconnecting from WiFi, turning off the display,
 * disabling external power, and turning off the LED. It can also be configured to wake up after a certain
 * number of seconds using the optional parameter.
 * 
 * @param seconds The number of seconds to sleep before waking up (default = 0).
 */
void heltec_deep_sleep(int seconds = 0) {
  #ifdef WiFi_h
    WiFi.disconnect(true);
  #endif
  display.displayOff();
  heltec_ve(false);
  heltec_led(0);
  #ifdef HELTEC_POWER_BUTTON
    // set to wake up on button press
    esp_sleep_enable_ext0_wakeup(BUTTON, LOW);
  #endif
  if (seconds > 0) {
    // set to wake up after a certain time
    esp_sleep_enable_timer_wakeup(seconds * 1000000);
  }
  // and off to bed
  esp_deep_sleep_start();
}

/**
 * @brief Calculates the battery percentage based on the measured battery voltage.
 * 
 * This function calculates the battery percentage based on the measured battery voltage.
 * If the battery voltage is not provided as a parameter, it will be measured using the heltec_vbat() function.
 * The battery percentage is then returned as an integer value.
 * 
 * @param vbat The battery voltage in volts (default = -1).
 * @return The battery percentage (0-100).
 */
int heltec_battery_percent(float vbat = -1) {
  if (vbat == -1) {
    vbat = heltec_vbat();
  }
  for (int n = 0; n < sizeof(scaled_voltage); n++) {
    float step = (max_voltage - min_voltage) / 256;
    if (vbat > min_voltage + (step * scaled_voltage[n])) {
      return 100 - n;
    }
  }
  return 0;
}

/**
 * @brief Checks if the device woke up from deep sleep due to button press.
 * 
 * @return True if the wake-up cause is a button press, false otherwise.
 */
bool heltec_wakeup_was_button() {
  return esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0;
}

/**
 * @brief Checks if the device woke up from deep sleep due to a timer.
 * 
 * This function checks if the device woke up from deep sleep due to a timer.
 * 
 * @return True if the wake-up cause is a timer interrupt, false otherwise.
 */
bool heltec_wakeup_was_timer() {
  return esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER;
}

/**
 * @brief Initializes the Heltec library.
 * 
 * This function initializes the Heltec library by setting up the necessary components,
 * such as the Serial communication, the display, and the power button.
 * If the power button feature is used, makes sure the device starts in off state after reset
 * or battery voltage is gone.
 */
void heltec_setup() {
  #ifdef HELTEC_POWER_BUTTON
    if (!heltec_wakeup_was_button() && !heltec_wakeup_was_timer()){
      heltec_deep_sleep();
    }
  #endif
  Serial.begin(115200);
  display.init();
  display.flipScreenVertically();
}

/**
 * @brief The main loop function for the Heltec library.
 * 
 * This function should be called in the main loop of the Arduino sketch.
 * It updates the state of the power button and implements long-press power off if used.
 */
void heltec_loop() {
  button.update();
  #ifdef HELTEC_POWER_BUTTON
    // Power off button checking
    if (button.isLongClick()) {
      // longClick is not long enough
      delay(300);
      if (digitalRead(BUTTON)) {
        return;
      }
      // Visually confirm it's off
      display.displayOff();
      // then wait for button to be released, 
      // or it will wake us right up again
      while (!digitalRead(BUTTON)) {
        delay(10);
      }
      heltec_deep_sleep();
    }
  #endif
}

/**
 * @brief Delays the execution of the program for the specified number of milliseconds.
 * 
 * This function delays the execution of the program for the specified number of milliseconds.
 * During the delay, it also calls the heltec_loop() function to allow for the power off
 * button to be checked.
 * 
 * @param ms The number of milliseconds to delay.
 */
void heltec_delay(int ms) {
  uint64_t start = millis();
  while (true) {
    heltec_loop();
    delay(1);
    if (millis() - start >= ms) {
      break;
    }
  }
}

#endif
