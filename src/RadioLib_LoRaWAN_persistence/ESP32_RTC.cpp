#include "ESP32_RTC.h"

RTC_DATA_ATTR uint8_t lorawan_nonces[RADIOLIB_LORAWAN_NONCES_BUF_SIZE];
RTC_DATA_ATTR uint8_t lorawan_session[RADIOLIB_LORAWAN_SESSION_BUF_SIZE];
RTC_DATA_ATTR int bootcount = 0;

bool restorePersistence(LoRaWANNode node) {
  if (bootcount++ == 0) {
    return false;
  }
  node.setBufferNonces(lorawan_nonces);
  node.setBufferSession(lorawan_session);
  return true;
}

bool savePersistence(LoRaWANNode node) {
  // allows recall of the session after deepsleep
  node.saveSession();
  memcpy(lorawan_nonces, node.getBufferNonces(), RADIOLIB_LORAWAN_NONCES_BUF_SIZE);
  memcpy(lorawan_session, node.getBufferSession(), RADIOLIB_LORAWAN_SESSION_BUF_SIZE);
  return true;
}
