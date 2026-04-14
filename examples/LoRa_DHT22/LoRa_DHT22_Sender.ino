/**
 * LoRa Sender with DHT22 sensor for Heltec ESP32 LoRa v3
 * Board: esp32 -> Heltec Wifi LoRa 32(V3)
 *
 * Reads temperature and humidity from a DHT22 sensor,
 * shows them on the OLED, and sends them over LoRa
 * in this payload format:
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
#include <DHT.h>

// ----- DHT22 -----
#define DHTPIN   4
#define DHTTYPE  DHT22
DHT dht(DHTPIN, DHTTYPE);

// ----- LoRa settings -----
#define FREQUENCY         915.0   // MHz
#define BANDWIDTH         125.0   // kHz
#define SPREADING_FACTOR  7
#define PREAMBLE_LENGTH   8
#define TX_OUTPUT_POWER   5       // dBm

// ----- App settings -----
#define SEND_INTERVAL_MS  30000UL   // 30 seconds
#define NODE_NAME         "Node0"

unsigned long lastSendTime = 0;
bool firstSend = true;

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
  display.drawString(0, 0, "DHT22 Sender");
  display.drawString(0, 12, "Starting...");
  display.display();

  Serial.println("DHT22 Sender");
  Serial.println("Radio init...");

  dht.begin();

  RADIOLIB_OR_HALT(radio.begin());

  Serial.printf("Frequency: %.1f MHz\n", FREQUENCY);
  RADIOLIB_OR_HALT(radio.setFrequency(FREQUENCY));

  Serial.printf("Bandwidth: %.1f kHz\n", BANDWIDTH);
  RADIOLIB_OR_HALT(radio.setBandwidth(BANDWIDTH));

  Serial.printf("SF: %d\n", SPREADING_FACTOR);
  RADIOLIB_OR_HALT(radio.setSpreadingFactor(SPREADING_FACTOR));

  Serial.printf("Preamble: %d\n", PREAMBLE_LENGTH);
  RADIOLIB_OR_HALT(radio.setPreambleLength(PREAMBLE_LENGTH));

  Serial.printf("TX Power: %d dBm\n", TX_OUTPUT_POWER);
  RADIOLIB_OR_HALT(radio.setOutputPower(TX_OUTPUT_POWER));

  Serial.println("Sender ready.");
  lastSendTime = millis();
}

void loop() {
  heltec_loop();

  unsigned long now = millis();

  if (!firstSend && (now - lastSendTime < SEND_INTERVAL_MS)) {
    return;
  }

  firstSend = false;
  lastSendTime = now;

  float tempF = dht.readTemperature(true);
  float humidity = dht.readHumidity();

  if (isnan(tempF) || isnan(humidity)) {
    Serial.println("Failed to read from DHT22!");

    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, "Sensor Error");
    display.drawString(0, 12, "Check wiring");
    display.display();
    return;
  }

  String payload = String(NODE_NAME) + "," + String(tempF, 1) + "," + String(humidity, 1);

  Serial.print("Sending: ");
  Serial.println(payload);

  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, String(NODE_NAME) + " Sending:");
  display.drawString(0, 12, "Temp: " + String(tempF, 1) + " F");
  display.drawString(0, 24, "Hum:  " + String(humidity, 1) + " %");
  display.display();

  int state = radio.transmit(payload);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("TX done.");
  } else {
    Serial.printf("TX failed, code %d\n", state);
  }
}