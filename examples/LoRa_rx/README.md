
# LoRa Receiver

This program configures a Heltec board to receive LoRa data at a specific frequency and configuration.

## Features:

- **Power Button**: The 'PRG' button acts as the power button for the board. Long pressing the button turns off the device.
- **LoRa Parameters**:
  - Frequency: 866.3 MHz (for Europe)
  - Bandwidth: 250 kHz
  - Spreading Factor: 9 (range: 5 to 12)

## Dependencies:

- [heltec_unofficial.h](https://github.com/decodeais/heltec_unofficial) (must be included before other Heltec libraries)
- [RadioLib](https://github.com/jgromes/RadioLib) for LoRa communication.

## Program Flow:

1. **Initialization**: The Heltec setup function (`heltec_setup()`) initializes the board, and the radio is set up using the `radio.begin()` function.
2. **Radio Configuration**:
   - The radio's frequency, bandwidth, and spreading factor are set.
   - The receiver is started with `radio.startReceive()` and a callback (`rx()`) is registered for when packets are received.
3. **Packet Reception**: When a packet is received:
   - The program reads the data and prints it along with the RSSI (signal strength) and SNR (signal-to-noise ratio).
   - The receiver restarts to wait for the next packet.

## Callback Function:

- `rx()`: This function is triggered whenever a LoRa packet is received and sets the `rxFlag` to `true` for processing in the main loop.

## License:

MIT License
