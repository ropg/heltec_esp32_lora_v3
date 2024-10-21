#define HELTEC_POWER_BUTTON   // must be before "#include <heltec_unofficial.h>"
#include <heltec_unofficial.h>
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi and MQTT configuration
const char* ssid = "XXXX";         // Deine WLAN SSID
const char* password = "XXXX"; // Dein WLAN-Passwort

String macAddressTopic;

WiFiClient espClient;
PubSubClient client(espClient);

// LoRa parameters
#define FREQUENCY           866.3    // Frequency in MHz (Europe)
#define BANDWIDTH           250.0    // LoRa bandwidth
#define SPREADING_FACTOR    9        // LoRa spreading factor
#define TRANSMIT_POWER      14       // Transmit power in dBm
long counter = 0;

// Function to connect to WiFi
void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Retrieve the MAC address and insert it into the MQTT topic
  String macAddress = WiFi.macAddress();
  macAddress.replace(":", "");  // Remove colons from the MAC address
  macAddressTopic = "heltec/" + macAddress + "/loradata";  // MQTT topic with MAC address

  // Print the topic name to the Serial Monitor
  Serial.print("MQTT Topic: ");
  Serial.println(macAddressTopic);
}

// MQTT callback function: called when a message is received
void callback(char* topic, byte* payload, unsigned int length) {
  String receivedMessage = "";
  for (int i = 0; i < length; i++) {
    receivedMessage += (char)payload[i];
  }
  Serial.print("Message received: ");
  Serial.println(receivedMessage);

  // Transmit the message via LoRa
  both.printf("TX [%s] ", receivedMessage.c_str());
  heltec_led(50);
  uint64_t tx_time = millis();
  RADIOLIB(radio.transmit(receivedMessage.c_str()));
  tx_time = millis() - tx_time;
  heltec_led(0);

  if (_radiolib_status == RADIOLIB_ERR_NONE) {
    both.printf("LoRa transmission OK (%i ms)\n", (int)tx_time);
  } else {
    both.printf("LoRa transmission failed (%i)\n", _radiolib_status);
  }
}

// Function to reconnect to MQTT broker
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("LoRaClient")) {
      Serial.println("connected");
      client.subscribe(macAddressTopic.c_str()); // Subscribe to the topic with the MAC address
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  // Initialize LoRa and WiFi
  heltec_setup();
  setup_wifi();

  client.setServer("test.mosquitto.org", 1883);
  client.setCallback(callback);

  both.println("TX Radio init");
  RADIOLIB_OR_HALT(radio.begin());

  // Set LoRa parameters
  both.printf("Frequency: %.2f MHz\n", FREQUENCY);
  RADIOLIB_OR_HALT(radio.setFrequency(FREQUENCY));

  both.printf("Bandwidth: %.1f kHz\n", BANDWIDTH);
  RADIOLIB_OR_HALT(radio.setBandwidth(BANDWIDTH));

  both.printf("Spreading Factor: %i\n", SPREADING_FACTOR);
  RADIOLIB_OR_HALT(radio.setSpreadingFactor(SPREADING_FACTOR));

  both.printf("TX power: %i dBm\n", TRANSMIT_POWER);
  RADIOLIB_OR_HALT(radio.setOutputPower(TRANSMIT_POWER));
}

void loop() {
  // Ensure MQTT stays connected
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Process incoming MQTT messages

  heltec_loop();
}
