// frequency range in MHz to scan
#define FREQ_BEGIN 850.00
#define FREQ_END 950.00

// Measurement bandwidth. Allowed bandwidth values (in kHz) are:
// 4.8, 5.8, 7.3, 9.7, 11.7, 14.6, 19.5, 23.4, 29.3, 39.0, 46.9, 58.6,
// 78.2, 93.8, 117.3, 156.2, 187.2, 234.3, 312.0, 373.6 and 467.0
#define BANDWIDTH 467.0

// (optional) major and minor tickmarks at x MHz
#define MAJOR_TICKS 10
// #define MINOR_TICKS 5

// Turns the 'PRG' button into the power button, long press is off 
#define HELTEC_POWER_BUTTON   // must be before "#include <heltec_unofficial.h>"
#include <heltec_unofficial.h>

// This file contains binary patch for the SX1262
#include "modules/SX126x/patches/SX126x_patch_scan.h"

// Prints the scan measurement bins from the SX1262 in hex
// #define PRINT_SCAN_VALUES

// Various other parameters
#define STEPS 128
#define SAMPLES 1024
#define MAJOR_TICK_LENGTH 3
#define MINOR_TICK_LENGTH 1
#define X_AXIS_WEIGHT 2
#define HEIGHT RADIOLIB_SX126X_SPECTRAL_SCAN_RES_SIZE
#define SCALE_TEXT_TOP (HEIGHT + X_AXIS_WEIGHT + MAJOR_TICK_LENGTH)
#define STATUS_TEXT_TOP (64 - 14)
#define RANGE (float)(FREQ_END - FREQ_BEGIN)
#define SINGLE_STEP (float)(RANGE / STEPS)

// Array to store the scan results
uint16_t result[RADIOLIB_SX126X_SPECTRAL_SCAN_RES_SIZE];

void setup() {
  heltec_setup();
  // initialize SX1262 FSK modem at the initial frequency
  both.println("Init radio");
  
  RADIOLIB_OR_HALT(radio.beginFSK(FREQ_BEGIN));
  // upload a patch to the SX1262 to enable spectral scan
  // NOTE: this patch is uploaded into volatile memory,
  //       and must be re-uploaded on every power up
  both.println("Upload patch");
  RADIOLIB_OR_HALT(radio.uploadPatch(sx126x_patch_scan, sizeof(sx126x_patch_scan)));
  // configure scan bandwidth and disable the data shaping
  both.println("Setting up radio");
  RADIOLIB_OR_HALT(radio.setRxBandwidth(BANDWIDTH));
  RADIOLIB_OR_HALT(radio.setDataShaping(RADIOLIB_SHAPING_NONE));
  both.println("Starting scan");
  display.clear();
  // Put everything on the display
  displayDecorate();
}

void loop() {
  // uint64_t start = millis();
  // clear the scan plot rectangle
  display.setColor(BLACK);
  display.fillRect(0,0,STEPS, HEIGHT);
  display.setColor(WHITE);
  // do the scan
  for (int x = 0; x < STEPS; x++) {
    float freq = FREQ_BEGIN + (RANGE * ((float) x / STEPS));
    radio.setFrequency(freq);
    // start spectral scan
    radio.spectralScanStart(SAMPLES, 1);
    // wait for spectral scan to finish
    while(radio.spectralScanGetStatus() != RADIOLIB_ERR_NONE) {
      heltec_delay(1);
    }
    // read the results
    radio.spectralScanGetResult(result);
    for (int y = 0; y < RADIOLIB_SX126X_SPECTRAL_SCAN_RES_SIZE; y++) {
      #ifdef PRINT_SCAN_VALUES
        Serial.printf("%04X,", result[y]);
      #endif
      if (result[y]) {
        display.setPixel(x, y);
      }
    }
    #ifdef PRINT_SCAN_VALUES
      Serial.println();
    #endif
    // wait a little bit before the next scan, otherwise the SX1262 hangs
    heltec_delay(9);
  }
  #ifdef PRINT_SCAN_VALUES
    Serial.println();
  #endif
  display.display();
  // Serial.printf("Scan took %lld ms\n", millis() - start);
}


/**
 * @brief Decorates the display: everything but the plot itself.
 */
void displayDecorate() {
  // begin and end ticks 
  display.fillRect(0, HEIGHT + X_AXIS_WEIGHT, 2, MAJOR_TICK_LENGTH);
  display.fillRect(126, HEIGHT + X_AXIS_WEIGHT, 2, MAJOR_TICK_LENGTH);
  // frequencies
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, SCALE_TEXT_TOP, String(FREQ_BEGIN));
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(127, SCALE_TEXT_TOP, String(FREQ_END));
  // "Scanning"
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(STEPS / 2, STATUS_TEXT_TOP, "Scanning");
  // X-axis
  display.fillRect(0, HEIGHT, STEPS, X_AXIS_WEIGHT);
  // ticks
  #ifdef MAJOR_TICKS
    drawTicks(MAJOR_TICKS, MAJOR_TICK_LENGTH);
  #endif
  #ifdef MINOR_TICKS
    drawTicks(MINOR_TICKS, MINOR_TICK_LENGTH);
  #endif
}

/**
 * @brief Draws ticks on the display at regular whole intervals.
 * 
 * @param every The interval between ticks in MHz. 
 * @param length The length of each tick in pixels.
 */
void drawTicks(float every, int length) {
  float first_tick  = FREQ_BEGIN + (every - (FREQ_BEGIN - (int)(FREQ_BEGIN / every) * every));
  if (first_tick < FREQ_BEGIN){
    first_tick += every;
  }
  for (float tick_freq = first_tick; tick_freq <= FREQ_END; tick_freq += every) {
    int tick = round((tick_freq - FREQ_BEGIN) / SINGLE_STEP);
    display.drawLine(tick, HEIGHT + X_AXIS_WEIGHT, tick, HEIGHT + X_AXIS_WEIGHT + length);
  }
}
