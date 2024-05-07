# LoRa Communication System

## Description
This project demonstrates a LoRa communication setup using Heltec WiFi LoRa 32 devices. It includes a sender and a receiver, facilitating bi-directional communication between the two. The system handles data transmission through LoRa, displays information on an OLED, and manages network connectivity and power states.

## Features
- **LoRa Communication:** Utilizes the LoRa protocol for low-power, long-range communication.
- **OLED Display:** Output information and status on an OLED display.
- **Power Management:** Includes deep sleep mode for energy efficiency.
- **Interrupt Handling:** Manages device functionality based on external interrupts.
- **CSV Data Parsing:** Parses and sends structured CSV data through LoRa.

## Hardware Requirements
- 2 x Heltec WiFi LoRa 32 (V2)
- Breadboard and connecting wires
- External sensors (if applicable for your setup)
- USB cables for programming

## Software Dependencies
- Arduino IDE
- Heltec ESP32 Library
- `WiFi.h`
- `LoRaWan_APP.h`
- `Wire.h`
- `HT_SSD1306Wire.h`
- Custom `CSV_Parser.h` for handling CSV data inputs

## Setup and Configuration
1. **Hardware Setup:**
   - Connect each Heltec WiFi LoRa 32 to your computer via USB.
   - Ensure that any external sensors or components are correctly wired to each board according to the project schematics.

2. **Software Setup:**
   - Install the Arduino IDE from [Arduino's official website](https://www.arduino.cc/en/software).
   - Add the Heltec ESP32 board to your Arduino IDE:
     - Go to File > Preferences, enter the following URL in Additional Board Manager URLs:
       ```
       https://dl.espressif.com/dl/package_esp32_index.json
       ```
     - Open Tools > Board > Boards Manager, search for "ESP32" and install.
   - Install the required libraries (`WiFi`, `LoRaWan_APP`, `Wire`, `HT_SSD1306Wire`) using the Library Manager in the Arduino IDE.

## Usage
1. **Programming the Devices:**
   - Open the sender and receiver sketches in the Arduino IDE.
   - Select the correct board from Tools > Board and choose the correct port from Tools > Port.
   - Upload the sender code to one Heltec device and the receiver code to another.

2. **Operating the System:**
   - Power the devices. They should automatically start sending and receiving data.
   - Observe the transmitted and received data on the OLED displays.

## Troubleshooting
- **Connection Issues:** Ensure that both devices are within the range of LoRa signal and that antennas are properly attached.
- **Display Issues:** Check the connections to the OLED display and ensure the `HT_SSD1306Wire` library is correctly installed.

## Contributing
Contributions to this project are welcome. Please fork the repository, make your changes, and submit a pull request.

## License
Specify the license under which your project is released.
