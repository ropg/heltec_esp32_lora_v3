/**
 * To use this, you need the ESPTelnet library from the Library Manager
 * 
 * This will output elapsed time since boot and battery voltage, both to
 * a party connecting via telnet and to the serial port. It is meant to
 * create a time-series to create the voltage curve of the battery as
 * used in heltec_unofficial.h to estimate battery percentage remaining.
 * 
 * (It's a telnet server because you can't discharge the battery with
 * USB-C plugged in.)
 * 
 * Save the resulting data and provide to the python script in src/tools
 * to get the constants in heltec_unofficial.h. The discharge curve I used is in
 * the same directory.
*/

#include <heltec_unofficial.h>
#include <WiFi.h>
#include "ESPTelnet.h"

#define SSID        "Your_SSID_here"
#define PASSWORD    "Your_password_here"
#define PORT        23

// seconds between updates
#define INTERVAL    60

uint64_t last_print;

ESPTelnet telnet;
uint16_t port = 23;

void setup() {
  // Set things up
  heltec_setup();
  // Use a little bit more power
  display.invertDisplay();
  // Connect to Wifi
  both.print("Wifi ");
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  do {
    delay(2000);
    both.print(".");
  } while (WiFi.status() != WL_CONNECTED);
  both.println(" connected.");
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  // Start the telnet server
  telnet.onConnect(onTelnetConnect);
  telnet.onConnectionAttempt(onTelnetConnectionAttempt);
  telnet.onReconnect(onTelnetReconnect);
  telnet.onDisconnect(onTelnetDisconnect);
  if (!telnet.begin(PORT)) {
    Serial.println("Error running telnet server.");
    while (true);
  }
  Serial.printf("Telnet to %s port %i.\n", WiFi.localIP().toString().c_str(), port);
  display.printf("Telnet to:\n  %s port %i.\n", WiFi.localIP().toString().c_str(), port);
}

void loop() {
  telnet.loop();

  // Print the battery voltage to serial and telnet every minute
  if (!last_print || millis() - last_print > INTERVAL * 1000) {
    int hr = millis() / 3600000;
    int min = (millis() / 60000) % 60;
    int sec = (millis() / 1000) % 60;
    float vbat = heltec_vbat();
    telnet.printf("%02d:%02d:%02d, %.2f\n", hr, min, sec, vbat);
    Serial.printf("%02d:%02d:%02d, %.2f\n", hr, min, sec, vbat);
    last_print = millis();
  }    
}

void onTelnetConnect(String ip) {
  Serial.printf("Telnet: %s connected.\n", ip.c_str());
  display.printf("Connected:\n  %s\n", ip.c_str());
  telnet.printf("Time, Voltage\n");
}

void onTelnetDisconnect(String ip) {
  Serial.printf("Telnet: %s disconnected.\n", ip.c_str());
}

void onTelnetReconnect(String ip) {
  Serial.printf("Telnet: %s reconnected.\n", ip.c_str());
}

void onTelnetConnectionAttempt(String ip) {
  Serial.printf("Telnet: %s tried to connect.\n", ip.c_str());
}
