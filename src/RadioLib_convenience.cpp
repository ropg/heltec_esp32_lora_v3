#include <Arduino.h>
#include "RadioLib.h"

int16_t _radiolib_status;

/**
 * @brief Short textual representation for a few common RadioLib status codes (e.g. "ERR_CHIP_NOT_FOUND".) Returns lookup URL for all others.
 * @param result The returned status code from RadioLib
 */
String radiolib_result_string(const int16_t result) {
  // Just a few common ones that people get often
  switch (result) {
  case RADIOLIB_ERR_NONE:
    return "ERR_NONE";
  case RADIOLIB_ERR_CHIP_NOT_FOUND:
    return "ERR_CHIP_NOT_FOUND";
  case RADIOLIB_ERR_PACKET_TOO_LONG:
    return "ERR_PACKET_TOO_LONG";
  case RADIOLIB_ERR_RX_TIMEOUT:
    return "ERR_RX_TIMEOUT";
  case RADIOLIB_ERR_CRC_MISMATCH:
    return "ERR_CRC_MISMATCH";
  case RADIOLIB_ERR_INVALID_BANDWIDTH:
    return "ERR_INVALID_BANDWIDTH";
  case RADIOLIB_ERR_INVALID_SPREADING_FACTOR:
    return "ERR_INVALID_SPREADING_FACTOR";
  case RADIOLIB_ERR_INVALID_CODING_RATE:
    return "ERR_INVALID_CODING_RATE";
  case RADIOLIB_ERR_INVALID_FREQUENCY:
    return "ERR_INVALID_FREQUENCY";
  case RADIOLIB_ERR_INVALID_OUTPUT_POWER:
    return "ERR_INVALID_OUTPUT_POWER";
  }
  return "See https://jgromes.github.io/RadioLib/group__status__codes.html";
}
