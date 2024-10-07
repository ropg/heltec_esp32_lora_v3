# I2C Scanner

This project is a simple example of using the I2C bus. It allows scanning for I2C devices connected to the I2C buses I2C0 and I2C1.

## Functionality

- The code uses the I2C bus to scan for connected devices. 
- if there is a OLED display connected to I2C0, and when scanning with `Wire` (I2C0), the return address should be `0x3C`.
- If you want to scan other devices on I2C1, comment out the `Wire` commands and uncomment the `Wire1` commands.

### Example Output

```text
Scanning...
I2C device found at address 0x3C  !
done
```

## Hardware Requirements
![Pins](https://github.com/decodeais/heltec_esp32_lora_v3/raw/main/images/heltec_esp32_lora_v3_pinout.png)
- Heltec ESP32 board (e.g., WiFi LoRa 32 V3)
- optional OLED display (e.g., SSD1306)

## Pin Mapping

- SCL (OLED): Pin 15
- SDA (OLED): Pin 4

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.
