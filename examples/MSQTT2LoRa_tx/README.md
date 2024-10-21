
# LoRa MQTT Bridge with MAC-based Topics

This project demonstrates how to create a LoRa-to-MQTT bridge using a Heltec ESP32 LoRa V3 module. The program connects to a WiFi network, retrieves the device's MAC address, subscribes to an MQTT topic based on the MAC address, and forwards any received messages via LoRa to another node.

## Features

- Connects to WiFi and MQTT broker
- Dynamically generates an MQTT topic using the board's MAC address in the format `heltec/{macAddress}/loradata`
- Receives MQTT messages and transmits them via LoRa to a receiver node
- Configurable LoRa parameters (frequency, bandwidth, spreading factor, and transmit power)

## Hardware Requirements

- Heltec ESP32 LoRa V3 module
- WiFi connection
- LoRa-compatible receiver node

## Software Requirements

- Heltec LoRa library
- PubSubClient library
- WiFi library (default for ESP32)

## Configuration

1. **WiFi Settings**:
   Update the `ssid` and `password` variables in the code to match your WiFi credentials:

   ```cpp
   const char* ssid = "your_wifi_ssid";
   const char* password = "your_wifi_password";
   ```

2. **MQTT Broker**:
   The project uses the public `test.mosquitto.org` broker by default, but you can change it if needed in the following line:

   ```cpp
   client.setServer("test.mosquitto.org", 1883);
   ```

3. **LoRa Settings**:
   You can adjust the LoRa parameters to comply with your region's frequency and desired communication settings:

   ```cpp
   #define FREQUENCY 866.3   // Frequency in MHz (Europe)
   #define BANDWIDTH 250.0   // LoRa bandwidth
   #define SPREADING_FACTOR 9 // Spreading factor
   #define TRANSMIT_POWER 14  // Transmit power in dBm
   ```

## Running the Project

1. Clone the repository or download the project files.
2. Open the project in your preferred IDE (e.g., Arduino IDE, PlatformIO).
3. Update the WiFi credentials and MQTT broker if necessary.
4. Flash the code to the Heltec ESP32 LoRa V3 module.
5. Open the Serial Monitor to check the connection status and see the generated MQTT topic (format: `heltec/{MAC address}/loradata`).
6. Once connected, the device will receive MQTT messages and transmit them via LoRa.

## Example Output

Here is an example output in the Serial Monitor after a successful connection an a me "Hello LoRa " published on the MSQTT-server:

```txt
Connecting to WiFi...
WiFi connected
MQTT Topic: heltec/A1B2C3D4E5F6/loradata
Attempting MQTT connection... connected
Message received: Hello LoRa
LoRa transmission OK (50 ms)
```

## Example of LoRa Receiver Output

The LoRa receiver node should be set up to listen for incoming LoRa messages. When a message is received, the output might look something like this in the Serial Monitor of the receiver node:

```txt
Received LoRa message: Hello LoRa
RSSI: -85 dBm
SNR: 9.5 dB
Frequency Error: 500 Hz
Message Length: 10 bytes
```

The data received over LoRa corresponds to the messages forwarded from the MQTT broker. Each message contains the content originally published to the MQTT topic, and the receiver node provides additional metadata such as:

- **RSSI** (Received Signal Strength Indicator): The strength of the received signal.
- **SNR** (Signal-to-Noise Ratio): A measure of the signal quality.
- **Frequency Error**: The difference between the expected and actual frequency.
- **Message Length**: The size of the received payload.

## License
This project is licensed under the MIT License. See the LICENSE file for more details.
