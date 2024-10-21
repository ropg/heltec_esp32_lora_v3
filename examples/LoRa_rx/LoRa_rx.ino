// Turns the 'PRG' button into the power button, long press is off 
#define HELTEC_POWER_BUTTON   // must be before "#include <heltec_unofficial.h>"
#include <heltec_unofficial.h>

// Frequency in MHz. Check your regulations for what is legal in your area.
#define FREQUENCY           866.3       // for Europe

// LoRa bandwidth.
#define BANDWIDTH           250.0

// Spreading factor from 5 to 12.
#define SPREADING_FACTOR    9

String rxdata;
volatile bool rxFlag = false;

void setup() {
  heltec_setup();
  both.println("RX Radio init");
  RADIOLIB_OR_HALT(radio.begin());
  
  // Set radio parameters
  both.printf("Frequency: %.2f MHz\n", FREQUENCY);
  RADIOLIB_OR_HALT(radio.setFrequency(FREQUENCY));
  
  both.printf("Bandwidth: %.1f kHz\n", BANDWIDTH);
  RADIOLIB_OR_HALT(radio.setBandwidth(BANDWIDTH));
  
  both.printf("Spreading Factor: %i\n", SPREADING_FACTOR);
  RADIOLIB_OR_HALT(radio.setSpreadingFactor(SPREADING_FACTOR));
  
  // Start receiving
  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  radio.setDio1Action(rx);  // Set callback for received packets
}

void loop() {
  heltec_loop();

  // If a packet was received, display it and the RSSI and SNR
  if (rxFlag) {
    rxFlag = false;
    // Stop the  receiver before reading new data ???????
    radio.standby();  // Setzt das Radio in den Standby-Modus
    radio.readData(rxdata);

    if (_radiolib_status == RADIOLIB_ERR_NONE) {
      both.printf("RX [%s]\n", rxdata.c_str());
      both.printf("  RSSI: %.2f dBm\n", radio.getRSSI());
      both.printf("  SNR: %.2f dB\n", radio.getSNR());
    }
    
    // Restart receiving after processing
    RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  }
}

// Callback function triggered when a packet is received
void rx() {
  rxFlag = true;
}
