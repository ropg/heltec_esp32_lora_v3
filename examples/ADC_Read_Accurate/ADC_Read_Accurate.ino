/*
  ADC read voltage with all ADC's with 1% accuracy.
 Modified the example from
            by Aaron.Lee from HelTec AutoMation, ChengDu, China
            成都惠利特自动化科技有限公司
            www.heltec.cn
  for the WiFi Lora 32(V3) board.
*/
// Arrays for ADC-Pins and their names
const int adcPins[] = { 1, 2, 3, 4, 5, 6, 7, 19, 20 };  // GPIO-Numbers of ADCPins
const char* adcPinNames[] = { "ADC1_CH0", "ADC1_CH1", "ADC1_CH2", "ADC1_CH3", "ADC1_CH4", "ADC1_CH5", "ADC1_CH6", "ADC2_CH8", "ADC2_CH9" };  // Bezeichnungen der ADC-Kanäle

void setup() {
  Serial.begin(115200);
}

void loop() {
  
  Serial.println("------------------Measurement------------------------");
   // Loop for all ADC-Pins
  for (int i = 0; i < sizeof(adcPins) / sizeof(adcPins[0]); i++) {
    int pin = adcPins[i];
    const char* pinName = adcPinNames[i];
    
    Serial.print(pinName);
    Serial.print(" (GPIO ");
    Serial.print(pin);
    Serial.print("): Voltage = ");
    Serial.print(ReadVoltage(pin), 3);
    Serial.print(" V, ADC-Value = ");
    Serial.println(analogRead(pin));
  }
  delay(1000);
}


double ReadVoltage(byte pin){
  double reading = analogRead(pin); // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
  if(reading < 1 || reading >= 4095)
    //return 0;
  // return -0.000000000009824 * pow(reading,3) + 0.000000016557283 * pow(reading,2) + 0.000854596860691 * reading + 0.065440348345433;
  return -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089;
} // Added an improved polynomial, use either, comment out as required

/* ADC readings v voltage
 *  y = -0.000000000009824x3 + 0.000000016557283x2 + 0.000854596860691x + 0.065440348345433
 // Polynomial curve match, based on raw data thus:
 *   464     0.5
 *  1088     1.0
 *  1707     1.5
 *  2331     2.0
 *  2951     2.5 
 *  3775     3.0
 *  
 */
