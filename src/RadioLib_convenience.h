#ifndef RadioLib_convenience_h
#define RadioLib_convenience_h

// Macros should also work on minimal platforms, so can't use Serial.printf()

/**
 * This allows for checking the off button, or whatever else the user wants to
 * do during a halt.
*/
#ifndef RADIOLIB_DO_DURING_HALT
  #define RADIOLIB_DO_DURING_HALT delay(10)
#endif

extern int16_t _radiolib_status;

/**
 * @brief Convenience macro for issuing a RadioLib command. This convenience
 *        macro can only be used for functions that return a status code.
 * @param action The RadioLib command to issue
*/
#define RADIOLIB(action) \
  _radiolib_status = action; \
  Serial.print("[RadioLib] "); \
  Serial.print(#action); \
  Serial.print(" returned "); \
  Serial.print(_radiolib_status); \
  Serial.print(" ("); \
  Serial.print(radiolib_result_string(_radiolib_status)); \
  Serial.println(")"); 

/**
 * @brief Convenience macro for issuing a RadioLib command and halting the
 *        program if the command fails. This convenience macro can only be used
 *        for RadioLib functions that return a status code.
 * @param action The RadioLib command to issue
*/
#define RADIOLIB_OR_HALT(action) \
  RADIOLIB(action); \
  if (_radiolib_status != RADIOLIB_ERR_NONE) { \
    Serial.println("[RadioLib] Halted"); \
    while (true) { \
        RADIOLIB_DO_DURING_HALT; \
    } \
  }

/**
 * @brief Short textual representation for a few common RadioLib status codes
 *        (e.g. "ERR_CHIP_NOT_FOUND".) Returns lookup URL for all others.
 * @param result The returned status code from RadioLib
 */
String radiolib_result_string(const int16_t result);

#endif
