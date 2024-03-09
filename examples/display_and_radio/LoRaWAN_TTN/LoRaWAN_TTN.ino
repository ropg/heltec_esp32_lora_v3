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

// JoinEUI - previous versions of LoRaWAN called this AppEUI
// for development purposes you can use all zeros - see wiki for details
uint64_t joinEUI = 0x0000000000000000;

// DevEUI - The device's Extended Unique Identifier
// TTN will generate one for you
uint64_t devEUI =  0x0000000000000000;

// encryption keys used to secure the communication
// TTN will generate them for you
// see wiki for details on copying & pasting them
uint8_t nwkKey[] = { 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--,   
                      0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x-- };
uint8_t appKey[] = { 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--,   
                      0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x-- };

// Pause between sends in seconds, so this is every 5 minutes. (Delay will be
// longer if regulatory or TTN Fair Use Policy requires it.)
#define MINIMUM_DELAY 300 

// Create a backup copy of the RTC RAM to flash every so many times
#define BACKUP_EVERY 100

// The LoRaWAN specification provides a port field (FPort) to distinguish
// between different types of messages on the receiving end. It's one byte, but
// FPort 0 is reserved for LoRaWAN-internal MAC messages, and values 224 through
// 255 (0xE0…0xFF) are reserved for future standardized application extensions,
// so don't use those.
#define FPORT 10


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

  // Give USB serial some time so we can always see the output
  delay (2000);

  // initialize radio
  both.println("Radio init");
  RADIOLIB(radio.begin());
  if (_radiolib_status != RADIOLIB_ERR_NONE) {
    goToSleep();  // Does not return, program starts over next round
  }

  // Manages uplink intervals to the TTN Fair Use Policy
  node.setDutyCycle(true, 1250);

  // Join the network, or resume previous saved session
  if (count == 1) {

    // If this is the first boot, we woke up with a wiped RTC RAM. We assume we
    // were rebooted, have lost RTC RAM (and thus the uplink packet counter
    // FCntUp) and need to join the network again with our keys and nonces saved
    // in flash.
    both.printf("Joining (forced)\n");
    RADIOLIB(node.beginOTAA(joinEUI, devEUI, nwkKey, appKey, RADIOLIB_LORAWAN_DATA_RATE_UNUSED, true));

  } else {

    // If we woke up with RTC RAM intact, continue with what we have.
    both.printf("Joining\n");
    RADIOLIB(node.beginOTAA(joinEUI, devEUI, nwkKey, appKey));

  }

  // Let's see what happened when we tried to join
  if (_radiolib_status == RADIOLIB_ERR_NONE) {

    both.println("Joined network");

  } else if (_radiolib_status == RADIOLIB_LORAWAN_MODE_OTAA){

    both.println("Was still joined");

  } else {

    // Something went wrong, print error and go to sleep
    display.printf("Join failed: %i\n  (%s)\n",
                   _radiolib_status,
                   radiolib_result_string(_radiolib_status).c_str()
                  );
    goToSleep();   // Does not return, program starts over next round

  }

  // If we're still here, it means we joined, and we can send something
  String strUp = "Hello! " + String(count);
  String strDown;
  both.printf("TX: %s\n", strUp.c_str());
  RADIOLIB(node.sendReceive(strUp, FPORT, strDown));
  if(_radiolib_status == RADIOLIB_ERR_NONE) {

    // We got a return packet!!
    handleReceived(strDown);

  } else if(_radiolib_status == RADIOLIB_ERR_RX_TIMEOUT) {

    // Don't be deceived by this being called 'ERR_RX_TIMEOUT', It's not an
    // error; not receiving anything in response to sending a packet is what
    // happens most of the time in LoRaWAN. So we do nothing special here.

  } else {

    // Print error 
    display.printf("TX failed: %i\n  (%s)\n",
                   _radiolib_status,
                   radiolib_result_string(_radiolib_status).c_str()
                  );
  }

  goToSleep();    // Does not return, program starts over next round

}

void loop() {

  // This is never called. There is no repetition: we always go back to deep
  // sleep one way or the other at the end of setup()

}


void handleReceived(String strDown) {

  // Set the flagh so we know to save the state to flash before going to sleep.
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

}

void goToSleep() {

  // allows recall of the session after deepsleep
  node.saveSession();

  // If we woke up with wiped RTC RAM, or received a message, or we've
  // reached BACKUP_EVERY, we back it up to flash before going to sleep.
  if (count == 1 || gotDownlink == true || count % BACKUP_EVERY == 0) {
    both.println("RTC RAM -> flash.");
    EEPROM.toNVS();
  }

  // Raise the boot counter, kept in RTC RAM
  count++;

  // Calculate minimum duty cycle delay (per FUP & law!)
  uint32_t interval = node.timeUntilUplink();

  // And then pick it or our MINIMUM_DELAY, whichever is greater
  uint32_t delayMs = max(interval, (uint32_t)MINIMUM_DELAY * 1000);
  
  both.printf("Next TX in %i s\n", delayMs/1000);

  // Give the user a chance to see the message
  both.println("Deep sleep in 5 s");
  delay(5000);

  // And off to bed we go
  heltec_deep_sleep((delayMs / 1000) - 5);

}
