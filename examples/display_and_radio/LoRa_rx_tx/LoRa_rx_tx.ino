/**
 * Send and receive LoRa-modulation packets with a sequence number,
 * showing RSSI and SNR for received packets on the little display.
*/

#include <heltec.h>

// Pause between transmited packets in seconds.
// Set to zero to only transmit a packet when pressing the user button
#define PAUSE               300

// Radio parameters. There's a decent explanation on 
// https://www.thethingsnetwork.org/docs/lorawan/spreading-factors/
#define FREQUENCY           869.5       // for Europe
#define BANDWIDTH           500
#define SPREADING_FACTOR    9
#define TRANSMIT_POWER      22

int state;
String rxdata;
volatile bool rxFlag = false;
long counter = 0;
uint64_t lastTX = 0;

void setup() {
  display.init();
  display.flipScreenVertically();
  display.print("Radio init ... ");
  state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    display.printf("ok\n");
  } else {
    display.printf("fail, code: %i\n", state);
    while (true);
  }

  // Set the callback function for received packets
  radio.setDio1Action(rx);

  // Set radio parameters
  radio.setOutputPower(TRANSMIT_POWER);
  radio.setFrequency(FREQUENCY);
  radio.setBandwidth(BANDWIDTH);
  radio.setSpreadingFactor(SPREADING_FACTOR);
  radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF);
}

void loop() {
  button.update();
  
  // Transmit a packet every PAUSE seconds or when the button is pressed
  if ((PAUSE && millis() - lastTX > (PAUSE * 1000)) || button.isSingleClick()) {
    display.printf("TX [%s] ", String(counter).c_str());
    radio.clearDio1Action();
    led(50); // 50% brightness is plenty for this LED
    state = radio.transmit(String(counter).c_str());
    led(0);
    lastTX = millis();
    radio.setDio1Action(rx);
    radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF);
    counter++;
    if (state == RADIOLIB_ERR_NONE) {
      display.printf("ok\n");
    } else {
      display.printf("fail, code: %i\n", state);
    }
  }

  // If a packet was received, display it and the RSSI and SNR
  if (rxFlag) {
    rxFlag = false;
    state = radio.readData(rxdata);
    if (state == RADIOLIB_ERR_NONE) {
      display.printf("RX [%s]\n", rxdata.c_str());
      display.printf(" RSSI: %.2f dBm\n", radio.getRSSI());
      display.printf(" SNR: %.2f dB\n", radio.getSNR());
    }
    radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF);
  }
}

// Can't do display things here, SPI takes too much time for the interrupt
void rx() {
  rxFlag = true;
}
