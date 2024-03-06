/*
  Deep Sleep LoRaWAN TTN example

  This example assumes the ESP32 stays powered, but puts it in a very low-power
  "deep sleep" mode between sends to the network.

  This example joins a LoRaWAN network and will send uplink packets. Before you
  start, you will have to register your device at
  https://www.thethingsnetwork.org/ After your device is registered, you can run
  this example. The device will join the network and start uploading data.

  LoRaWAN v1.1 requires storing parameters persistently. RadioLib does this by
  using the Arduino EEPROM storage. The ESP32 does not have EEPROM memory, so
  the Arduino code from Espressif has code that emulates EEPROM in its "NVS"
  flash key-value storage. This unfortunately means that every change of a
  single but re- writes the entire emulated "EEPROM" to flash again. 

  That would be great for the occasional configuration change, but we need to
  keep state after every single interaction with the LoRaWAN network. Flash
  memory can only be erased and written to a limited number of times. This is
  why this library contains a fork of RadioLib that uses ESP32_RTC_EEPROM, which
  writes to the memory of the ESP32's onboard Real-Time Clock which is kept
  powered during the chip's "deep sleep", which uses almost no power.

  This example then backs up that RTC RAM memory to flash
    a) every time it is woken up with a wiped RTC RAM,
    b) whenever it receives a downlink packet, or 
    c) every so many times as set by BACKUP_EVERY.

  This backup is then automatically loaded when the RTC RAM is empty (after a
  reset or power failure) so that we hang on to enough state information to
  rejoin the network in a secure manner. 

  For default module settings, see the wiki page
  https://github.com/jgromes/RadioLib/wiki/Default-configuration

  For full API reference, see the GitHub Pages
  https://jgromes.github.io/RadioLib/

  For LoRaWAN details, see the wiki page
  https://github.com/jgromes/RadioLib/wiki/LoRaWAN
*/

  // encryption keys used to secure the communication
  // TTN will generate them for you
  // see wiki for details on copying & pasting them
  uint8_t nwkKey[] = { 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--,   
                       0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x-- };
  uint8_t appKey[] = { 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--,   
                       0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x-- };
// Create a backup copy of the RTC RAM to flash every so many times
#define BACKUP_EVERY 100

#include <heltec.h>

// create the node instance on the EU-868 band
// using the radio module and the encryption key
// make sure you are using the correct band
// based on your geographical location!
LoRaWANNode node(&radio, &EU868);

// for fixed bands with subband selection
// such as US915 and AU915, you must specify
// the subband that matches the Frequency Plan
// that you selected on your LoRaWAN console
// LoRaWANNode node(&radio, &US915, 2);

// Variables that are placed in RTC RAM survive deep sleep. This counter tells
// us how many times we've booted since the last reset or power loss.
RTC_DATA_ATTR int count = 1;

void setup() {
  heltec_setup();

  // Give USB serial some time so we always see the output
  delay (2000);

  String strUp;
  String strDown;
  bool gotDownlink = false;

  // initialize radio
  both.println("Radio init");
  RADIOLIB(radio.begin());
  if (_radiolib_status != RADIOLIB_ERR_NONE) {
    goto sleep;
  }

  // Manages uplink intervals to the TTN Fair Use Policy
  node.setDutyCycle(true, 1250);

  // join the network, or resume previous saved session
  if (count == 1) {
    both.printf("Joining (forced)\n");
    RADIOLIB(node.beginOTAA(joinEUI, devEUI, nwkKey, appKey, RADIOLIB_LORAWAN_DATA_RATE_UNUSED, true));
  } else {
    both.printf("Joining\n");
    RADIOLIB(node.beginOTAA(joinEUI, devEUI, nwkKey, appKey));
  }
  if (_radiolib_status == RADIOLIB_ERR_NONE) {
    display.println("Joined network");
  } else if (_radiolib_status == RADIOLIB_LORAWAN_MODE_OTAA){
    display.println("Was still joined");
  } else {
    display.printf("Join fail: %i\n", _radiolib_status);
    goto sleep;
  }

  // send uplink to port 10
  strUp = "Hello! " + String(count++);
  both.printf("TX: %s\n", strUp.c_str());
  RADIOLIB(node.sendReceive(strUp, 10, strDown));
  if(_radiolib_status == RADIOLIB_ERR_NONE) {
    gotDownlink = true;
    both.print("RX: ");
    // print data of the packet (if there are any)
    if(strDown.length() > 0) {
      both.println(strDown);
    } else {
      both.println("<MAC cmds only>");
    }

    // print RSSI (Received Signal Strength Indicator)
    both.printf("  RSSI: %.1f dBm\n", radio.getRSSI());

    // print SNR (Signal-to-Noise Ratio)
    both.printf("  SNR: %.1f dB\n", radio.getSNR());

    // // print frequency error
    // both.printf("  Freq err: %.1f Hz\n", radio.getFrequencyError());
  
  } else if(_radiolib_status == RADIOLIB_ERR_RX_TIMEOUT) {
    // Nothing, not receiving anything is what happens most of the time
  
  } else {
    both.printf("fail: %i\n", _radiolib_status);
  }

  // allows recall of the session after deepsleep
  node.saveSession();
  
  // If we woke up with wiped RTC RAM, or received a message, or we've
  // reached BACKUP_EVERY, we back it up to flash.
  if (count == 1 || gotDownlink == true || count % BACKUP_EVERY == 0) {
    Serial.println("RTC RAM -> flash.");
    EEPROM.toNVS();
  }

  count++;

  // Label to jump to from failed beginOTAA, so we try again later
  sleep:

  // wait before sending another packet
  uint32_t minimumDelay = 300000;                 // try to send once every 5 minutes
  uint32_t interval = node.timeUntilUplink();     // calculate minimum duty cycle delay (per FUP & law!)
  uint32_t delayMs = max(interval, minimumDelay); // cannot send faster than duty cycle allows
  both.printf("Next TX in %i s\n", delayMs/1000);

  both.println("Deep sleep in 5 s");
  delay(5000);
  heltec_deep_sleep((delayMs / 1000) - 5);
}

void loop() {
  // this code will never be reached
}
