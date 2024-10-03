
# ADC_Read_Accurate

This project demonstrates how to read accurate voltages using the ADC pins of the Heltec WiFi Lora 32 (V3) board with approximately 1% accuracy. It has been adapted from an example by Aaron Lee from HelTec Automation, ChengDu, China.

## Features

- Reads voltage from multiple ADC pins (ADC1 and ADC2).
- Provides a polynomial function for more accurate voltage calculations.
- Displays the voltage and corresponding ADC value in the serial monitor.

## Hardware

- **Board**: Heltec WiFi Lora 32 (V3)
  ![Pins](../images/heltec_esp32_lora_v3_pinout.png)
  
- **ADC Pins**: GPIO pins mapped to the ADC channels as follows:
  - ADC1 Channels: `ADC1_CH0` to `ADC1_CH6`
  - ADC2 Channels: `ADC2_CH8`, `ADC2_CH9`

## Code Overview

### ADC Pins and Names

The code uses the following arrays to manage the ADC pins and their names:

```cpp
const int adcPins[] = { 1, 2, 3, 4, 5, 6, 7, 19, 20 };  // GPIO numbers of ADC pins
const char* adcPinNames[] = { "ADC1_CH0", "ADC1_CH1", "ADC1_CH2", "ADC1_CH3", "ADC1_CH4", "ADC1_CH5", "ADC1_CH6", "ADC2_CH8", "ADC2_CH9" };
```

### Polynomial Voltage Calculation

To improve the accuracy of the voltage readings, the code includes a polynomial function that converts the raw ADC reading to voltage. Two different polynomials are provided. The default one used is of the fourth order for better precision.

```cpp
double ReadVoltage(byte pin) {
  double reading = analogRead(pin); // Reference voltage is 3.3V, max ADC value is 4095
  return -0.000000000000016 * pow(reading, 4) 
         + 0.000000000118171 * pow(reading, 3)
         - 0.000000301211691 * pow(reading, 2)
         + 0.001109019271794 * reading 
         + 0.034143524634089;
}
```

### Example Output

The code prints the voltage and corresponding ADC value for each ADC pin to the serial monitor.

Example:

```text
------------------Measurement------------------------
ADC1_CH0 (GPIO 1): Voltage = 0.500 V, ADC-Value = 464
ADC1_CH1 (GPIO 2): Voltage = 1.000 V, ADC-Value = 1088
...
```

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## Acknowledgments

Adapted from an example by Aaron Lee from HelTec Automation, ChengDu, China.


[def]: images/Heltec_Wireless_Stick_Lite_v3_schematic.png