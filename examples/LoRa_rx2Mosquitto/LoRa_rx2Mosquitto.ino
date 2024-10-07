#include <heltec_unofficial.h>
#include <WiFi.h>
#include <PubSubClient.h>

// WLAN credentials
const char* ssid = "xxxx";         // Your WiFi SSID
const char* password = "xxxx";     // Your WiFi password

// MQTT server details
const char* mqtt_server = "test.mosquitto.org";  // Free Mosquitto MQTT server
const int mqtt_port = 1883;                      // Default port for unencrypted communication

WiFiClient espClient;
PubSubClient client(espClient);

// Frequency in MHz. Check your regulations for what is legal in your area.
#define FREQUENCY           866.3       // for Europe

// LoRa bandwidth.
#define BANDWIDTH           250.0

// Spreading factor from 5 to 12.
#define SPREADING_FACTOR    9

String rxdata;
volatile bool rxFlag = false;
String macAddressTopic;

// Function for WiFi connection
void setup_wifi() {
  delay(10);
  both.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    both.print(".");
  }

  both.println("WiFi connected.");
  both.println(WiFi.localIP());
}

// Function for connecting to the MQTT server
void reconnect() {
  while (!client.connected()) {
    both.println("Attempting MQTT connection...");
    if (client.connect("HeltecESP32Client")) {
      both.println("connected to MQTT server");
    } else {
      both.print("failed, rc=");
      both.print(client.state());
      both.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  heltec_setup();
  both.println("Radio init");
  RADIOLIB_OR_HALT(radio.begin());
  setup_wifi();  // Connect to WiFi

  // Retrieve the MAC address of the board and insert it into the MQTT topic
  String macAddress = WiFi.macAddress();
  macAddress.replace(":", "");  // Remove colons from the MAC address
  macAddressTopic = "heltec/" + macAddress + "/loradata";
  
  // Output the topic name on the serial console
  both.print("MQTT Topic: ");
  both.println(macAddressTopic);

  client.setServer(mqtt_server, mqtt_port);  // Set MQTT server
  
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

  if (!client.connected()) {
    reconnect();  // Restore MQTT connection
  }
  
  client.loop();  // Call MQTT client loop

  // If a packet was received, display it and send it to the MQTT broker
  if (rxFlag) {
    rxFlag = false;

    // Stop receiver before reading data
    radio.standby();  // Set the radio to standby mode

    radio.readData(rxdata);

    if (_radiolib_status == RADIOLIB_ERR_NONE) {
      both.printf("RX [%s]\n", rxdata.c_str());
      both.printf("  RSSI: %.2f dBm\n", radio.getRSSI());
      both.printf("  SNR: %.2f dB\n", radio.getSNR());

      // Send data to MQTT server
      
      client.publish((macAddressTopic + "/RX").c_str(), rxdata.c_str());  // Send data to the dynamic topic
      client.publish((macAddressTopic + "/RSSI").c_str(), String(radio.getRSSI()).c_str());  // Send data to the dynamic topic
      client.publish((macAddressTopic + "/SNR").c_str(), String(radio.getSNR()).c_str());  // Send data to the dynamic topic
    }
    
    // Restart receiving after processing
    RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  }
}

// Callback function triggered when a packet is received
void rx() {
  rxFlag = true;
}
