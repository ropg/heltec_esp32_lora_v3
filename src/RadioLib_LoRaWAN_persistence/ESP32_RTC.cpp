#include "ESP32_RTC.h"

// The Arduino Prefeences library is used to store data in NVS, Espressif's key-value flash store
#include "Preferences.h"
Preferences nvs;

// Storage in RTC RAM
RTC_DATA_ATTR uint8_t lorawan_nonces[RADIOLIB_LORAWAN_NONCES_BUF_SIZE];
RTC_DATA_ATTR uint8_t lorawan_session[RADIOLIB_LORAWAN_SESSION_BUF_SIZE];
RTC_DATA_ATTR int bootcount = 0;

bool restorePersistence(LoRaWANNode &node) {
  if (bootcount++ == 0) {
    RADIOLIB_DEBUG_PROTOCOL_PRINTLN("[restorePersistence] bootcount == 0");
    // If we woke up fresh, restore only the nonces from flash
    nvs.begin("lorawan");
    if (nvs.getBytes("nonces", lorawan_nonces, RADIOLIB_LORAWAN_NONCES_BUF_SIZE) == RADIOLIB_LORAWAN_NONCES_BUF_SIZE) {
      RADIOLIB_DEBUG_PROTOCOL_PRINTLN("[restorePersistence] Nonces restored from NVS");
      node.setBufferNonces(lorawan_nonces);
    } else {
      RADIOLIB_DEBUG_PROTOCOL_PRINTLN("[restorePersistence] No nonces found in NVS");
    }
    nvs.end();
    return false;
  } else {
    // If this is a repeated boot, restore nonces and session data from RTC RAM
    RADIOLIB_DEBUG_PROTOCOL_PRINTLN("[restorePersistence] Nonces and session data restored from RTC RAM");
    node.setBufferNonces(lorawan_nonces);
    node.setBufferSession(lorawan_session);
    return true;
  }
}

bool savePersistence(LoRaWANNode &node) {
  // Get the persistence data from RadioLib and copy to RTC RAM
  memcpy(lorawan_nonces, node.getBufferNonces(), RADIOLIB_LORAWAN_NONCES_BUF_SIZE);
  memcpy(lorawan_session, node.getBufferSession(), RADIOLIB_LORAWAN_SESSION_BUF_SIZE);
  RADIOLIB_DEBUG_PROTOCOL_PRINTLN("[savePersistence] Nonces and session data saved to RTC RAM");
  // Store the nonces in flash
  nvs.begin("lorawan");
  if (nvs.putBytes("nonces", lorawan_nonces, RADIOLIB_LORAWAN_NONCES_BUF_SIZE) == RADIOLIB_LORAWAN_NONCES_BUF_SIZE) {
    RADIOLIB_DEBUG_PROTOCOL_PRINTLN("[savePersistence] Nonces saved to NVS. (Only actually written if changed.)");
    nvs.end();
    return true;
  } else {
    RADIOLIB_DEBUG_PROTOCOL_PRINTLN("[savePersistence] There was a problem saving Nonces to NVS");
    nvs.end();
    return false;
  }
}
