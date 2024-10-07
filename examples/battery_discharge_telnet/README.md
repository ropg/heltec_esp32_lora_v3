
# ESP32 Battery Monitoring with Telnet Server

This project monitors the battery voltage of an ESP32-based device and outputs the elapsed time since boot and battery voltage to both a Telnet client and the serial port. The goal is to create a time series of battery voltage data to estimate the remaining battery percentage based on a discharge curve.

## Prerequisites

To use this project, you will need the following:

- ESP32 development board (such as Heltec ESP32 LoRa V3)
- [ESPTelnet library](https://github.com/LennartHennigs/ESPTelnet) (available via Arduino IDE Library Manager)
- WiFi connection

## How it works

- The program creates a Telnet server to monitor the battery voltage over time. Since you can't discharge the battery while connected via USB-C, Telnet is used to output the data wirelessly.
- The battery voltage data is captured and printed every 60 seconds.
- The data is saved and later processed using the provided Python script in `src/tools` to determine the constants for `heltec_unofficial.h`.

## Setup

1. Install the necessary libraries using the Arduino IDE Library Manager.
2. Flash the code to your ESP32 board.
3. Ensure that your WiFi credentials are added correctly in the source code:

```cpp
#define SSID        "Your_SSID_here"
#define PASSWORD    "Your_password_here"
```

4. Once connected, you can access the device via Telnet using the IP address shown in the serial monitor or the device's display.

## Telnet Instructions

- Use any Telnet client to connect to the device. The default port is `23`.
- The data output includes the elapsed time and the current battery voltage in the format `hh:mm:ss, voltage`.

## Example Output

```
Telnet to 192.168.1.100 port 23.
Time, Voltage
00:01:00, 3.85
00:02:00, 3.84
00:03:00, 3.83
```

## Data Processing

- After gathering data from the Telnet session, save it to a file.
- Use the provided Python script located in `src/tools` to analyze the discharge curve and calculate constants for estimating the battery percentage.

## Notes

- The program uses the `heltec_unofficial.h` library for battery voltage reading and setup.
- WiFi auto-reconnect is enabled for stability.

## License

This project is licensed under the MIT License.
