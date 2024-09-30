# Introduction

[PlatformIO](https://platformio.org/) is a versatile and feature-rich development environment widely used for programming ESP microcontrollers, such as the [ESP32](https://www.espressif.com/en/products/socs/esp32) and [ESP8266](https://www.espressif.com/en/products/socs/esp8266). Unlike the [Arduino IDE](https://www.arduino.cc/en/software), which is often seen as beginner-friendly but limited in features, PlatformIO offers a more robust ecosystem with support for multiple platforms, integrated libraries, and tools like debugging and unit testing. It simplifies dependency management, automates library installations, and provides an advanced editor with IntelliSense, code completion, and version control integration. PlatformIO also allows more efficient compilation, build customization, and a seamless workflow across different operating systems, making it ideal for more complex projects and professional development setups.

To enable the use of PlatformIO for this project, this directory contains code of an Arduino based example, adapted for PlatformIO.

## How to Install PlatformIO

PlatformIO can be installed either as a standalone IDE or as an extension for [Visual Studio Code](https://code.visualstudio.com/).

### 1. Standalone Installation
To install the standalone version of PlatformIO, download the installer for your operating system directly from the [official PlatformIO website](https://platformio.org/install/ide?install=vscode). This version comes bundled with everything needed for development, offering an all-in-one solution.

### 2. Installing PlatformIO as a VSCode Extension
If you prefer to use Visual Studio Code, you can install PlatformIO as an extension. Open VSCode and go to the Extensions view (`Ctrl+Shift+X` or `Cmd+Shift+X` on macOS). Search for "PlatformIO IDE" and click **Install**. Once installed, PlatformIO will integrate directly into VSCode, providing a powerful development environment.

For more detailed instructions on installation, refer to the [official PlatformIO Installation Guide](https://platformio.org/install).

## Migrating Arduino-based projects to PlatformIO

When migrating Arduino-based projects to PlatformIO, you may encounter compatibility issues due to differences in how PlatformIO handles libraries, board configurations, and certain Arduino-specific functions. For example, PlatformIO uses its own library management system, so library versions may differ from those in the Arduino IDE, potentially causing compilation errors. Additionally, board definitions and settings like pin mappings or clock frequencies may need adjustments in the `platformio.ini` file to match the original Arduino setup. To resolve these issues, ensure that the correct libraries and versions are declared in your `platformio.ini` file and manually add any missing libraries or dependencies. 

You can look at the Arduino IDE's compiler messages to find out which libraries and dependencies are used in a project. PlatformIO also allows you to specify the Arduino framework by setting `framework = arduino` in the `platformio.ini` file, ensuring better compatibility. Reviewing and adjusting platform-specific configurations can often solve the majority of issues when porting an Arduino project. For further assistance, PlatformIO’s extensive [documentation](https://docs.platformio.org/en/latest/) and community support are valuable resources.


## Example

This repository contains an example of a PlatformIO project. It is based on the Arduino example provided in the TTN Mapper project. The example is adapted to PlatformIO and the ESP32 platform.

