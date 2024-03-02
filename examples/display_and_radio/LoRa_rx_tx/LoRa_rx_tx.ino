/**
 * Send and receive LoRa-modulation packets with a sequence number,
 * showing RSSI and SNR for received packets on the little display.
 *
 * Note that while this send and received using LoRa modulation,
 * it does not do LoRaWAN, see the LoRaWAN_TTN example for that.
*/

// Turns the 'PRG' button into the power button, long press is off 
#define HELTEC_POWER_BUTTON   // must be before "#include <heltec.h>"
#include <heltec.h>

// Pause between transmited packets in seconds.
// Set to zero to only transmit a packet when pressing the user button
// Will not exceed 1% duty cycle, even if you set a lower value.
#define PAUSE               300

// Check your own rules and regulations to see what is legal where you are.
#define FREQUENCY           866.3       // for Europe
// #define FREQUENCY           905.2       // for US

// LoRa bandwidth. Keep the decimal point to designate float.
// Allowed values are 7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125.0, 250.0 and 500.0 kHz.
#define BANDWIDTH           250.0

// Number from 5 to 12. Higher means slower but higher "processor gain",
// meaning (in nutshell) longer range and more robust against interference. 
#define SPREADING_FACTOR    9

// Transmit power in dBm. 14 dBm = 25 mW, which is the legal maximum ERP in the EU ISM bands.
// The story is more complex and also depends on your antenna.
#define TRANSMIT_POWER      14

String rxdata;
volatile bool rxFlag = false;
long counter = 0;
uint64_t last_tx = 0;
uint64_t tx_time;
uint64_t minimum_pause;

void setup() {
  heltec_setup();
  both.println("Radio init");
  RADIO_OR_HALT(begin());
  // Set the callback function for received packets
  radio.setDio1Action(rx);
  // Set radio parameters
  both.printf("Frequency: %.2f MHz\n", FREQUENCY);
  RADIO_OR_HALT(setFrequency(FREQUENCY));
  both.printf("Bandwidth: %.1f kHz\n", BANDWIDTH);
  RADIO_OR_HALT(setBandwidth(BANDWIDTH));
  both.printf("Spreading Factor: %i\n", SPREADING_FACTOR);
  RADIO_OR_HALT(setSpreadingFactor(SPREADING_FACTOR));
  both.printf("TX power: %i dBm\n", TRANSMIT_POWER);
  RADIO_OR_HALT(setOutputPower(TRANSMIT_POWER));
  // Start receiving
  RADIO_OR_HALT(startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
}

void loop() {
  heltec_loop();
  
  bool tx_legal = millis() > last_tx + minimum_pause;
  // Transmit a packet every PAUSE seconds or when the button is pressed
  if ((PAUSE && tx_legal && millis() - last_tx > (PAUSE * 1000)) || button.isSingleClick()) {
    // In case of button click, tell user to wait
    if (!tx_legal) {
      both.printf("Legal limit, wait %i sec.\n", ((minimum_pause - (millis() - last_tx)) / 1000) + 1);
      return;
    }
    both.printf("TX [%s] ", String(counter).c_str());
    radio.clearDio1Action();
    heltec_led(50); // 50% brightness is plenty for this LED
    tx_time = millis();
    RADIO(transmit(String(counter++).c_str()));
    tx_time = millis() - tx_time;
    heltec_led(0);
    if (radio_result == RADIOLIB_ERR_NONE) {
      both.printf("OK (%i ms)\n", tx_time);
    } else {
      both.printf("fail (%i)\n", radio_result);
    }
    // Maximum 1% duty cycle
    minimum_pause = tx_time * 100;
    last_tx = millis();
    radio.setDio1Action(rx);
    RADIO_OR_HALT(startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  }

  // If a packet was received, display it and the RSSI and SNR
  if (rxFlag) {
    rxFlag = false;
    radio.readData(rxdata);
    if (radio_result == RADIOLIB_ERR_NONE) {
      both.printf("RX [%s]\n", rxdata.c_str());
      both.printf("  RSSI: %.2f dBm\n", radio.getRSSI());
      both.printf("  SNR: %.2f dB\n", radio.getSNR());
    }
    RADIO_OR_HALT(startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  }
}

// Can't do Serial pr display things here, takes too much time for the interrupt
void rx() {
  rxFlag = true;
}
