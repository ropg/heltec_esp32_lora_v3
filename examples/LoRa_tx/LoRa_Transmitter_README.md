
# LoRa Transmitter

This program configures the board to send LoRa packets at regular intervals or when the user button is pressed.

## Features

- **PRG Button as Power Button**:
 The PRG button is reconfigured to act as a power button.
- **LoRa Transmission**: Sends LoRa packets at a specified frequency and transmission power.
- **Adjustable Pause Between Packets**: Configure a pause between transmissions, or transmit only on button press.
- **TX Power and Bandwidth**: Adjustable LoRa bandwidth, spreading factor, and transmission power.
- **LED Indicator**: The onboard LED flashes during transmission.

## Requirements

- Heltec ESP32 LoRa V3 module
- `heltec_unofficial.h` library
- `RadioLib` library

## Configuration

- **Frequency**: 866.3 MHz (Europe)
- **Bandwidth**: 250 kHz
- **Spreading Factor**: 9
- **Transmit Power**: 14 dBm (25mW)
- **Pause**: 300 seconds between packets or transmit on button press.

## Usage

The program will automatically begin transmitting packets after setting up the radio. To modify the behavior:

- Adjust the `PAUSE` value to change the interval between packets.
- Change the frequency, bandwidth, or transmission power as needed.

To build and upload the code, follow standard Arduino procedures for ESP32.

## License

This program is open-source, feel free to modify and distribute.
