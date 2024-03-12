# Heltec ESP32 LoRa v3

## The unofficial library

![](images/ESP32_LoRa_v3.png)

### Introduction

There's this Chinese company named Heltec, and they make a cool little development board that has an Espressif ESP32S3 (which has WiFi and Bluetooth), a 128x64 pixel OLED display and an SX1262 863-928 MHz radio on it. It sells under different names on the internet, but internally they call it **[HTIT-WB32LA](images/heltec_esp32_lora_v3_documentation.pdf)**. (They have a 470-510 MHz version also, called HTIT-WB32LAF.) The hardware is cool, the software that comes with it is not so much my taste. There's multiple GitHub repositories, it's initailly unclear what is what, they use some radio stack of unknown origin, code-quality and documentation varies, some examples need tinkering and what could be a cool toy could easily become a very long weekend of frustration before things sort of work.

This library allows you to use that time to instead play with this cool board. The examples are tested, and this library assumes that for all things sub-GHz, you want to use the popular RadioLib.

&nbsp;

### The great Heltec board confusion

There is some level of general confusion when it comes to Heltec devices. They make a bewildering array of stuff, and it's not always clear (to me at least) what they call certain pieces of hardware and what specs this hardware has. This library is presently for two devices.

* I started making it for something they call "**WiFi LoRa 32(V3)**", "HTIT-WB32LA", or "ESP32 LoRa v3". The board has a USB-C connector, an SX1262 radio, a 128 x 64 OLED display and an ESP32S3 processor. It's the thing on the left in the picture above.

* I have since gotten hold of a "**Wireless Stick(V3)**", with very similar specs and an even smaller 64 x 32 OLED display. It's almost the same on the electrical side of things, the most notable exception being that the power of the display is now connected to "external power" that needs to be explicitly turned on with a GPIO pin. This device is now also supported by this library. It's the device on the right in the image.

> There is apparently a "**Wireless Stick Lite (v3)**" that just lacks the little OLED screen. This library **may or may not work** fine with it, I don't have one so I haven't checked that.

This library is unlikely to work as is with any other devices, made by heltec or others. You may still be able to modify it, or use ideas or whole chunks of code from it, but just **know that this library is known to work with the two pictured boards only**.

For purposes of clarity, I might speak about "the regular board" and "the stick" to mean the two supported devices.

#### `#define HELTEC_WIRELESS_STICK`

If you have the regular board, all the examples and this library work out of the box. If you have a stick, you **must** put `#define HELTEC_WIRELESS_STICK` **before** `#include <heltec.h>` or things will not work for you.

> _Main symptom of things not working on the stick is jerky and slow serial output while you are printing to `both` (see below) as the device waits for SPI timeouts from an OLED display that doesn't have power._

&nbsp;


##### (Click blue headings for more detailed documentation)

### Setting up

Make sure your Arduino IDE knows about ESP-32 boards by putting the following URL under "Additional board manager URLs" in the Arduino IDE settings:

```
https://espressif.github.io/arduino-esp32/package_esp32_index.json
```

Then under "*Settings / Board*" select "*Heltec WiFi LoRa 32(V3) / Wireless shell (V3) / Wireless stick lite (V3)*". Install this library using the Arduino library manager, or by cloning this repository in your Arduino libraries directory.

&nbsp;

### Getting started

In your sketch, `#include <heltec.h>`. This will provide the display, radio and button instances. Then in your `setup()`, put `heltec_setup()` to initialize the serial port at 115.200 bps and initialize the display. In the `loop` part of your sketch, put `heltec_loop()`. This will make sure the button is scanned, and provides the [deep sleep "off"](#using-it-as-the-power-button) functionality if you set that up.

```cpp
#include <heltec.h>

void setup() {
  heltec_setup();

  [...]
}

void loop() {
  heltec_loop();

  [...]
}
```

&nbsp;

If you `#define HELTEC_NO_RADIO_INSTANCE` and/or `#define HELTEC_NO_DISPLAY_INSTANCE` before `#include <heltec.h>`, you get no instances of `radio` and/or `display`, so you can set these up manually. Note that the library then also doesn't turn things off at sleep, etc.

&nbsp;

### [RadioLib](https://jgromes.github.io/RadioLib/)

This library includes my [fork of RadioLib](https://github.com/ropg/RadioLib). This is because that fork uses my [ESP32_RTC_EEPROM](https://github.com.ropg/ESP32_RTC_EEPROM) when compiled on ESP32, allowing for much less wear on the ESP32 flash. RadioLib plans to have a more generic mechanism allowing for the retention of state information and as soon as that's in there, this library will depend on (and thus auto-install) the latest version of RadioLib instead of including a copy of it. As long as this uses my fork, make sure the original version of RadioLib is uninstalled to avoid the compiler getting confused.

Next to the radio examples in this library, all [RadioLib examples](https://github.com/jgromes/RadioLib/tree/master/examples) that work with an SX1262 work here. Simply `#include <heltec.h>` instead of RadioLib and remove any code that creates a `radio` instance, it already exists when you include this library.

> * _It might otherwise confuse you at some point: while Heltec wired the DIO1 line from the SX1262 to the ESP32 (as they should, it is the interrupt line), they labeled it in their `pins_arduino.h` and much of their own software as DIO0. The SX1262 IO pins start at DIO1._
> * _If you place `#define HELTEC_NO_RADIOLIB` before `#include <heltec.h>`, RadioLib will not be included and this library won't create a radio object. Handy if you are not using the radio and need the space in flash for something else or if you want to use another radio library or so._

&nbsp;

##### Convenience macros: `RADIOLIB()` and `RADIOLIB_OR_HALT()`

This library provides convenience macros when calling RadioLib functions. It can be used for those functions that return a status code. When your code calls

```cpp
RADIOLIB_OR_HALT(radio.setFrequency(866.3));
```
this gets translated into

```cpp
  _radiolib_status = radio.setFrequency(866.3);
  _radiolib_status = action;
  Serial.print("[RadioLib] ");
  Serial.print("radio.setFrequency(866.3)");
  Serial.print(" returned ");
  Serial.print(_radiolib_status);
  Serial.print(" (");
  Serial.print(radiolib_result_string(_radiolib_status));
  Serial.println(")");
  if (_radiolib_status != RADIOLIB_ERR_NONE) {
    Serial.println("[RadioLib] Halted");
    while (true) {
        heltec_loop();
    }
  }
```

In other words, this saves a whole lot of typing if what you want is for RadioLib functions to be called and serial debug output to be generated. Calling `RADIOLIB` instead of `RADIOLIB_OR_HALT` does the same thing without the halting. 

> * _The `heltec_loop()` part in `RADIOLIB_OR_HALT` makes sure that if you have set the `PRG` button to be the power button, it still works when execution is halted._
> * _`_radiolib_status` is an integer that the library provides and that your code can check afterwards to see what happened._
> * _`_radiolib_result_string()` returns a textual representation (e.g. `CHIP_NOT_FOUND`) for a few of the most common errors or a URL to look up the others._

&nbsp;

### [Display](https://github.com/ThingPulse/esp8266-oled-ssd1306)

The tiny OLED display uses the same library that the original library from Heltec uses, except now the examples work so you don't have to figure out how to make things work. It is included inside this library because the Heltec board needs a hardware reset and I adapted some things to make the Arduino `print` functionality work better. (The latter change [submitted](https://github.com/ThingPulse/esp8266-oled-ssd1306/pull/389) to the original library also.)

There's the primary display library and there's an additinal UI library that allows for multiple frames. The display examples will show you how things work. The library, courtesy of ThingPulse, is well-written and well-documented. [Check them out](https://thingpulse.com/) and buy their stuff.

##### Printing to both Serial and display: `both.print()`

Instead of using `print`, `println` or `printf` on either `Serial` or `display`, you can also print to `both`. As the name implies, this prints the same thing on both devices. You'll find it used in many of this library's examples.

&nbsp;

### [Button](https://github.com/ropg/HotButton)

The user button marked 'PRG' on the regular board and 'USER' on the stick is handled by my own HotButton library that comes with this one. Since we have only one button it makes sense to be able to do as many different things with it as possible. It provides the generic `button.isSingleClick()` and `button.isDoubleClick()`, but it can do much more than that. I recommed having a quick look at its (short) [documentation](https://github.com/ropg/HotButton) to see what it can do.

Remember to put `heltec.loop()` in the`loop()` of your sketch to make sure your button gets updated so you can use these functions.

##### Using it as the power button

If you hook up this board to power, and especially if you hook up a LiPo battery (see below), you'll notice there's no on/off switch. Luckily the ESP32 comes with a very low-power "deep sleep" mode where it draws so little current it can essentially be considered off. Since signals on GPIO pins can wake it back up, we can use the button on the board as a power switch. In your sketch, simply put **`#define HELTEC_POWER_BUTTON`** before `#include <heltec.h>`, make sure `heltec_loop()` is in your own `loop()` and then a button press will wake it up and a long press will turn it off. You can still use `button.isSingleClick()` and `button.isDoubleClick()` in your `loop()` function when you use it as a power button.

> * _If you use `delay()` in your code, the power off function will not work during that delay. To fix that, simply use **`heltec_delay()`** instead._

&nbsp;

### [Deep Sleep](https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/)

You can use `heltec_deep_sleep(<seconds>)` to put the board into this 'off' deep sleep state yourself. This will put the board in deep sleep for the specified number of seconds. After it wakes up, it will run your sketch from the start again. You can use `heltec_wakeup_was_button()` and `heltec_wakeup_was_timer()` to find out whether the wakeup was caused by the power button or because your specified time has elapsed. You can even hold on to some data in variables that survive deep sleep by tagging them `RTC_DATA_ATTR`. More is in [this tutorial](https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/).

In deep sleep, with this library, according my multimeter power consumption drops to **147 µA** on the regular board (**141 µA** on the stick) if you have defined `HELTEC_POWER_BUTTON`, or **24 µA** on either board if you only use the timer to wake up. Please let me know if you can get it lower than that.

> * _If you call `heltec_deep_sleep()` without a number in seconds when not using the power button feature, you will need to reset it to turn it back on. Note that resetting does reinitialize any `RTC_DATA_ATTR` variables._
> * _The`deep_sleep_tester` example lets you easily put it in the four essential deep sleep modes: no wakeup other than reset, clock-wakeup, button-wakeup or both. I may be doing somthing wrong, but I don't measure any difference between having clock wakeup enabled and having no wakeup sources._

&nbsp;

### LED

The board has a bright white LED, next to the orange power/charge LED. This library provides a function `heltec_led()` that takes the LED brightness in percent. It's really bight, you'll probably find 50% brightness is plenty.

&nbsp;

### Battery

The board is capable of charging a LiPo battery connected to the little 2-pin connector at the bottom. `heltec_vbat()` gives you a float with the battery voltage, `heltec_battery_percent()` provides the estimated percentage full.

Note that it takes a single cell (3.7 V) LiPo and that the plus is on the left side when holding the board with the USB-C connector facing up.

![](images/battery-connector.jpg)

> * _According to the [schematic](images/heltec_esp32_lora_v3_schematic.pdf), the charge current is set to 500 mA. There's a voltage measuring setup where if GPIO37 is pulled low, the battery voltage appears on GPIO1. (Resistor-divided: VBAT - 390kΩ - GPIO1 - 100kΩ - GND)_
> * _You can optionally provide the float that `heltec_vbat()` returns to `heltec_battery_percent()` to make sure both are based on the same measurement._
> * _The [charging IC](images/tp4054.pdf) used will charge the battery to ~4.2V, then hold the voltage there until charge current is 1/10 the set current (i.e. 50 mA) and then stop and let it discharge to 4.05V (about 90%) and then charge it again, so this is expected._
> * _The orange charging LED, on but very dim is no battery is plugged in, is awfully bright when charging, and the IC on the reverse side of the reset switch gets quite hot when the battery is charging but still fairly empty. It's limited to 100 ℃ so nothing too bad can happen, just so you know._

The battery percentage estimate in this library is based on a real LiPo discharge curve.

![](/images/battery_curve.png)

The library contains all the tools to measure your own curve and use it instead, see [`heltec.h`](src/heltec.h) for details.

&nbsp;

### Ve - external power

There's two pins marked 'Ve' that are wired together and connected to a GPIO-controlled FET that can source 350 mA at 3.3V to power sensors etc. Turn on by calling `heltec_ve(true)`, `heltec_ve(false)` turns it off.

On the stick, this is also what powers the OLED display. This libary turns it on when initializing when you have set `#define HELTEC_WIRELESS_STICK` before `#include <heltec.h>`

> _(Not that they told anyone they hooked the display to "external power", so that's one afternoon I will never get back.)_

&nbsp;

&nbsp;

## Minimal example to show everything:

```cpp
// Turns the 'PRG' button into the power button, long press is off 
#define HELTEC_POWER_BUTTON   // must be before "#include <heltec.h>"

// Uncomment this if you have Wireless Stick v3
// #define HELTEC_WIRELESS_STICK

// creates 'radio', 'display' and 'button' instances 
#include <heltec.h>

void setup() {
  heltec_setup();
  Serial.println("Serial works");
  // Display
  display.println("Display works");
  // Radio
  display.print("Radio ");
  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    display.println("works");
  } else {
    display.printf("fail, code: %i\n", state);
  }
  // Battery
  float vbat = heltec_vbat();
  display.printf("Vbat: %.2fV (%d%%)\n", vbat, heltec_battery_percent(vbat));
}

void loop() {
  heltec_loop();
  // Button
  if (button.isSingleClick()) {
    display.println("Button works");
    // LED
    for (int n = 0; n <= 100; n++) { heltec_led(n); delay(5); }
    for (int n = 100; n >= 0; n--) { heltec_led(n); delay(5); }
    display.println("LED works");
  }
}
```

For a more meaningful demo, especially if you have two of these boards, [check out `LoRa_rx_tx`](examples/display_and_radio/LoRa_rx_tx/LoRa_rx_tx.ino) in the examples. The [`LoRaWAN_TTN` example](examples/display_and_radio/LoRaWAN_TTN/LoRaWAN_TTN.ino) works, uses [The Things Network](https://www.thethingsnetwork.org/) and goes to deep sleep between sends.

&nbsp;

## Reference

### Heltec_ESP32_LoRa_v3 / "the regular board"

![](images/heltec_esp32_lora_v3_pinout.png)

[![](images/heltec_esp32_lora_v3_schematic.png)](https://github.com/ropg/heltec_esp32_lora_v3/blob/main/images/heltec_esp32_lora_v3_schematic.pdf)

&nbsp;

### Heltec_Wireless_Stick_v3_pinout / "the stick"

![](images/Heltec_Wireless_Stick_v3_pinout.jpg)

[![](images/Heltec_Wireless_Stick_v3_schematic.png)](images/Heltec_Wireless_Stick_v3_schematic.pdf)

&nbsp;

***If you read this far, would you please star this repository?***
*(Not so much for my ego, but it helps other people find it. Thanks!)*
