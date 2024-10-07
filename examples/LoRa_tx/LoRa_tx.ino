// Turns the 'PRG' button into the power button, long press is off 
#define HELTEC_POWER_BUTTON   // must be before "#include <heltec_unofficial.h>"
#include <heltec_unofficial.h>

// Pause between transmitted packets in seconds. Set to zero to transmit only when pressing the user button.
#define PAUSE               300

// Frequency in MHz. Check your regulations for what is legal in your area.
#define FREQUENCY           866.3       // for Europe

// LoRa bandwidth.
#define BANDWIDTH           250.0

// Spreading factor from 5 to 12.
#define SPREADING_FACTOR    9

// Transmit power in dBm. 14dBm for 25mW.
#define TRANSMIT_POWER      14

long counter = 0;
uint64_t last_tx = 0;
uint64_t tx_time;
uint64_t minimum_pause;

void setup() {
  heltec_setup();
  both.println("TX  Radio init");
  RADIOLIB_OR_HALT(radio.begin());
  
  // Set radio parameters
  both.printf("Frequency: %.2f MHz\n", FREQUENCY);
  RADIOLIB_OR_HALT(radio.setFrequency(FREQUENCY));
  
  both.printf("Bandwidth: %.1f kHz\n", BANDWIDTH);
  RADIOLIB_OR_HALT(radio.setBandwidth(BANDWIDTH));
  
  both.printf("Spreading Factor: %i\n", SPREADING_FACTOR);
  RADIOLIB_OR_HALT(radio.setSpreadingFactor(SPREADING_FACTOR));
  
  both.printf("TX power: %i dBm\n", TRANSMIT_POWER);
  RADIOLIB_OR_HALT(radio.setOutputPower(TRANSMIT_POWER));
}

void loop() {
  heltec_loop();

  bool tx_legal = millis() > last_tx + minimum_pause;

  // Transmit a packet every PAUSE seconds or when the button is pressed
  if ((PAUSE && tx_legal && millis() - last_tx > (PAUSE * 1000)) || button.isSingleClick()) {
    if (!tx_legal) {
      both.printf("Legal limit, wait %i sec.\n", (int)((minimum_pause - (millis() - last_tx)) / 1000) + 1);
      return;
    }

    both.printf("TX [%s] ", String(counter).c_str());
    heltec_led(50); // 50% brightness is plenty for this LED
    tx_time = millis();
    RADIOLIB(radio.transmit(String(counter++).c_str()));
    //RADIOLIB(radio.transmit("Hallo"));
    tx_time = millis() - tx_time;
    heltec_led(0);

    if (_radiolib_status == RADIOLIB_ERR_NONE) {
      both.printf("OK (%i ms)\n", (int)tx_time);
    } else {
      both.printf("fail (%i)\n", _radiolib_status);
    }

    // Maximum 1% duty cycle
    minimum_pause = tx_time * 100;
    last_tx = millis();
  }
}
