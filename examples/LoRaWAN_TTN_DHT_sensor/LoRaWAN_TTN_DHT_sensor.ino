/*

  Deep Sleep LoRaWAN TTN with DHT temperaure and humidity sensor.


  This example focuses on making a real-world usable sensor that uses LoRaWAN /
  TTN. To keep it readable, the comments explaining how to get on TTN have been
  removed. I suggest you play with the LoRaWAN_TTN example first and then use
  this as a template if you want to build a real-world endpoint later.

  To run this, you will need a DHT-11 sensor wired to GND, 3.3V and GPIO 19.
  Also, you'll need to use the library manager to install the Adafruit DHT
  library ( https://github.com/adafruit/DHT-sensor-library ). These sensors
  sometimes ship on little break-out boards that say +5V at the power pin, but
  they are fine on 3.3V.)

  This example assumes the ESP32 has a LiPo battery attached. It is set up to
  send one measurement of temperature, humidity and battery percentage to The
  Things Network every 30 minutes. These are sent as 5 byte packets:

    First byte  : whole degrees Celsius

    Second byte : Fractional degrees Celsius (0-99) 

    Third byte  : whole percent relative humidity (RH)

    Fourth byte : fractional RH (0-99)

    Fifth byte  : Battery percentage (0-100)

  This sensor is ready to be deployed: it does not put anything on the OLED
  screen, it just wakes up, reads the sensor, sends the bytes and falls asleep
  for half an hour again. At the time of writing this, we have deep sleep
  without button wakeup down to 24 µA, so the battery should last a very long
  time indeed.

*/

// The same network data that you used in the LoRaWAN_TTN example.
uint64_t joinEUI = 0x0000000000000000;
uint64_t devEUI =  0x0000000000000000;
uint8_t nwkKey[] = { 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--,   
                      0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x-- };
uint8_t appKey[] = { 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--,   
                      0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x-- };


// The Adafruit DHT library
#include "DHT.h"

// The GPIO pin the sensor is hooked up to
#define DHTPIN 19

#define DHTTYPE DHT11   // DHT 11
// #define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
// #define DHTTYPE DHT21   // DHT 21 (AM2301)

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);


// Pause between sends in seconds, so this is every 30 minutes.
#define MINIMUM_DELAY 1800 

// Create a backup copy of the RTC RAM to flash every so many times
#define BACKUP_EVERY 100

// The LoRaWAN specification provides a port field (FPort) to distinguish
// between different types of messages on the receiving end. It's one byte, but
// FPort 0 is reserved for LoRaWAN-internal MAC messages, and values 224 through
// 255 (0xE0…0xFF) are reserved for future standardized application extensions,
// so don't use those.
#define FPORT 10

#define HELTEC_WIRELESS_STICK
#include <heltec.h>

// create the node instance on the EU-868 band using the radio module and the
// encryption key make sure you are using the correct band based on your
// geographical location!
LoRaWANNode node(&radio, &EU868);

// for fixed bands with subband selection such as US915 and AU915, you must
// specify the subband that matches the Frequency Plan that you selected on your
// LoRaWAN console LoRaWANNode node(&radio, &US915, 2);

// Variables that are placed in RTC RAM survive deep sleep. This counter tells
// us how many times we've booted since the last reset or power loss.
RTC_DATA_ATTR int count = 1;

// This flag is set when we receive a downlink packet, so we can save to flash.
bool gotDownlink = false;

  
void setup() {
  heltec_setup();

  dht.begin();

  // initialize radio
  Serial.println("Radio init");
  RADIOLIB(radio.begin());
  if (_radiolib_status != RADIOLIB_ERR_NONE) {
    goToSleep();  // Does not return, program starts over next round
  }

  // Manages uplink intervals to the TTN Fair Use Policy
  node.setDutyCycle(true, 1250);

  // We tell the LoRaWAN code about our battery level in case the network
  // asks for battery-level and SNR with a MAC_DEV_STATUS message.
  // ( 0 for external power source, 1 for lowest battery, 254 for highest battery,
  // 255 for unable to measure.)
  node.setDeviceStatus(((float)heltec_battery_percent() * 2.53) + 1);

  // Join the network, or resume previous saved session
  if (count == 1) {
    // If this is the first boot, we woke up with a wiped RTC RAM. We assume we
    // were rebooted, have lost RTC RAM (and thus the uplink packet counter
    // FCntUp) and need to join the network again with our keys and nonces saved
    // in flash.
    Serial.printf("Joining (forced)\n");
    RADIOLIB(node.beginOTAA(joinEUI, devEUI, nwkKey, appKey, RADIOLIB_LORAWAN_DATA_RATE_UNUSED, true));
  } else {
    // If we woke up with RTC RAM intact, continue with what we have.
    Serial.printf("Joining\n");
    RADIOLIB(node.beginOTAA(joinEUI, devEUI, nwkKey, appKey));
  }

  // Let's see what happened when we tried to join
  if (_radiolib_status != RADIOLIB_ERR_NONE && _radiolib_status != RADIOLIB_LORAWAN_MODE_OTAA) {
    Serial.printf("Join failed, trying again next time.");
    goToSleep();   // Does not return, program starts over next round
  }

  // If we're still here, it means we joined, and we can send something
  sendPacket();
  goToSleep();    // Does not return, program starts over next round
}

void loop() {

  // This is never called. There is no repetition: we always go back to deep
  // sleep one way or the other at the end of setup()

}

void sendPacket() {
  // 5-byte packet
  uint8_t packet[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Temperatue and Humidity
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    // just leaves the 0xFFs in the packets
  } else {
    Serial.printf("Temperature %.2f°C, Relative humidity %.2f%%\n", t, h);
    packet[0] = int(t);
    packet[1] = int((float)(t - packet[0]) * 100);
    packet[2] = int(h);
    packet[3] = int((float)(h - packet[2]) * 100);
  }

  // Battery percentage
  int bat_pct = heltec_battery_percent();
  Serial.printf("Battery percentage %d%%\n", bat_pct);
  packet[4] = bat_pct;

  // Send the packet. If no RX_TIMEOUT, it may mean we got a downlink packet, so
  // we store to flash before going to sleep.
  if (node.sendReceive(packet, 5, FPORT) == RADIOLIB_ERR_NONE) {
    gotDownlink = true;
  }
}

void goToSleep() {

  // allows recall of the session after deepsleep
  node.saveSession();

  // If we woke up with wiped RTC RAM, or received a message, or we've
  // reached BACKUP_EVERY, we back it up to flash before going to sleep.
  if (count == 1 || gotDownlink == true || count % BACKUP_EVERY == 0) {
    Serial.println("RTC RAM -> flash.");
    EEPROM.toNVS();
  }

  // Raise the boot counter, kept in RTC RAM
  count++;

  // Calculate minimum duty cycle delay (per FUP & law!)
  uint32_t interval = node.timeUntilUplink();

  // And then pick it or our MINIMUM_DELAY, whichever is greater
  uint32_t delayMs = max(interval, (uint32_t)MINIMUM_DELAY * 1000);
  
  Serial.printf("Next TX in %i s\n", delayMs/1000);

  Serial.println("Deep sleep now");
  // Make sure this still prints before sleep
  delay(10);

  // And off to bed we go
  heltec_deep_sleep((delayMs / 1000) - 5);

}
