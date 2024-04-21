/**
 * 
 * FOR THIS EXAMPLE TO WORK, YOU MUST INSTALL THE "LoRaWAN_ESP32" LIBRARY USING
 * THE LIBRARY MANAGER IN THE ARDUINO IDE.
 * 
 * This code will send a two-byte LoRaWAN message every 15 minutes. The first
 * byte is a simple 8-bit counter, the second is the ESP32 chip temperature
 * directly after waking up from its 15 minute sleep in degrees celsius + 100.
 *
 * If your NVS partition does not have stored TTN / LoRaWAN provisioning
 * information in it yet, you will be prompted for them on the serial port and
 * they will be stored for subsequent use.
 *
 * See https://github.com/ropg/LoRaWAN_ESP32
*/


// Pause between sends in seconds, so this is every 15 minutes. (Delay will be
// longer if regulatory or TTN Fair Use Policy requires it.)
#define MINIMUM_DELAY 900 


#include <heltec_unofficial.h>
#include <LoRaWAN_ESP32.h>

#include "driver/temp_sensor.h"

LoRaWANNode* node;

RTC_DATA_ATTR uint8_t count = 0;

void setup() {
  heltec_setup();

  // Obtain directly after deep sleep
  // May or may not reflect room temperature, sort of. 
  float temp = ESP32_temp();
  Serial.printf("Temperature: %.2f °C\n", temp);

  // initialize radio
  Serial.println("Radio init");
  uint16_t state = radio.begin();
  if (state != RADIOLIB_ERR_NONE) {
    Serial.println("Radio did not initialize. We'll try again later.");
    goToSleep();
  }

  node = persist.manage(&radio);

  if (!node->isJoined()) {
    Serial.println("Could not join network. We'll try again later.");
    goToSleep();
  }

  // If we're still here, it means we joined, and we can send something


  // Manages uplink intervals to the TTN Fair Use Policy
  node->setDutyCycle(true, 1250);

  uint8_t uplinkData[2];
  uplinkData[0] = count;
  uplinkData[1] = temp + 100;

  uint8_t downlinkData[256];
  size_t lenDown = sizeof(downlinkData);

  state = node->sendReceive(uplinkData, sizeof(uplinkData), 1, downlinkData, &lenDown);

  if(state == RADIOLIB_ERR_NONE || state == RADIOLIB_ERR_RX_TIMEOUT) {
    Serial.println("Message sent");
  } else {
    Serial.printf("sendReceive returned error %d, we'll try again later.\n", state);
  }

  goToSleep();    // Does not return, program starts over next round

}

void loop() {
  // This is never called. There is no repetition: we always go back to
  // deep sleep one way or the other at the end of setup()
}

void goToSleep() {
  Serial.println("Going to deep sleep now");
  // allows recall of the session after deepsleep
  persist.saveSession(node);
  // Calculate minimum duty cycle delay (per FUP & law!)
  uint32_t interval = node->timeUntilUplink();
  // And then pick it or our MINIMUM_DELAY, whichever is greater
  uint32_t delayMs = max(interval, (uint32_t)MINIMUM_DELAY * 1000);
  Serial.printf("Next TX in %i s\n", delayMs/1000);
  delay(100);  // So message prints

  esp_sleep_enable_timer_wakeup(delayMs * 1000);

  // INSERT WHATEVER ELSE YOU NEED TO DO TO MINIMIZE POWER USAGE DURING SLEEP
  
  // and off to bed we go
  esp_deep_sleep_start();
}


// This gets the ESP32 chip temperature, automatically using the best
// offset for the current temperature.
float ESP32_temp() {
  // We start with the coldest range, because those temps get spoiled 
  // the quickest by heat of processor waking up. 
  temp_sensor_dac_offset_t offsets[5] = {
    TSENS_DAC_L4,   // (-40°C ~  20°C, err <3°C)
    TSENS_DAC_L3,   // (-30°C ~  50°C, err <2°C)
    TSENS_DAC_L2,   // (-10°C ~  80°C, err <1°C)
    TSENS_DAC_L1,   // ( 20°C ~ 100°C, err <2°C)
    TSENS_DAC_L0    // ( 50°C ~ 125°C, err <3°C)
  };
  // If temperature for given n below this value,
  // then this is the best measurement we have.
  int cutoffs[5] = { -30, -10, 80, 100, 2500 };
  float result = 0;
  for (int n = 0; n < 5; n++) {
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    temp_sensor.dac_offset = offsets[n];
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
    temp_sensor_read_celsius(&result);
    temp_sensor_stop();
    // Serial.printf("L%d: %f°C\n", 4 - n, result);
    if (result < cutoffs[n]) break;
  }
  return result;
}
