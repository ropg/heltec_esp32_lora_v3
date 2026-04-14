/**
 * LoRa Receiver for DHT22 payloads on Heltec ESP32 LoRa v3
 * Board: esp32 -> Heltec Wifi LoRa 32(V3)
 *
 * Receives LoRa packets in this payload format:
 *
 *   Node0,72.5,48.1
 *
 * and shows them on the OLED and Serial.
 *
 * Manual VEXT power and explicit display init are used because OLED stayed dark on my board with heltec_setup() alone.
 *
 */

#define HELTEC_POWER_BUTTON

#include <heltec_unofficial.h>

// ----- LoRa settings -----
#define FREQUENCY         915.0   // MHz
#define BANDWIDTH         125.0   // kHz
#define SPREADING_FACTOR  7
#define PREAMBLE_LENGTH   8

String rxdata;
volatile bool rxFlag = false;

void onReceiveInterrupt() {
  rxFlag = true;
}

void setup() {
  Serial.begin(115200);

  // Force OLED power rail on first. Fixes screen going black.
  pinMode(VEXT, OUTPUT);
  digitalWrite(VEXT, LOW);
  delay(100);

  heltec_setup();

  // Re-init display explicitly. Fixes screen going black.
  display.init();
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "DHT22 Receiver");
  display.drawString(0, 12, "Starting...");
  display.display();

  Serial.println("DHT22 Receiver");
  Serial.println("Radio init...");

  RADIOLIB_OR_HALT(radio.begin());

  Serial.printf("Frequency: %.1f MHz\n", FREQUENCY);
  RADIOLIB_OR_HALT(radio.setFrequency(FREQUENCY));

  Serial.printf("Bandwidth: %.1f kHz\n", BANDWIDTH);
  RADIOLIB_OR_HALT(radio.setBandwidth(BANDWIDTH));

  Serial.printf("SF: %d\n", SPREADING_FACTOR);
  RADIOLIB_OR_HALT(radio.setSpreadingFactor(SPREADING_FACTOR));

  Serial.printf("Preamble: %d\n", PREAMBLE_LENGTH);
  RADIOLIB_OR_HALT(radio.setPreambleLength(PREAMBLE_LENGTH));

  radio.setDio1Action(onReceiveInterrupt);

  Serial.println("Receiver ready.");

  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Receiver Ready");
  display.drawString(0, 12, "Waiting...");
  display.display();

  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
}

void loop() {
  heltec_loop();

  if (!rxFlag) {
    return;
  }

  rxFlag = false;
  rxdata = "";

  int state = radio.readData(rxdata);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.printf("Received: %s | RSSI: %.2f | SNR: %.2f\n",
                  rxdata.c_str(),
                  radio.getRSSI(),
                  radio.getSNR());

    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, "RX: " + rxdata);
    display.drawString(0, 12, "RSSI: " + String(radio.getRSSI(), 1));
    display.drawString(0, 24, "SNR: " + String(radio.getSNR(), 1));
    display.display();
  } else {
    Serial.printf("RX failed, code %d\n", state);

    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, "RX failed");
    display.drawString(0, 12, "Code: " + String(state));
    display.display();
  }

  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
}