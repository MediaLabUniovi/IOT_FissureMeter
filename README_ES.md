<div align="right">

[![english](https://raw.githubusercontent.com/stevenrskelton/flag-icon/master/png/16/country-4x3/gb.png)](./README.md) | [![spanish](https://raw.githubusercontent.com/stevenrskelton/flag-icon/master/png/16/country-4x3/es.png)](./README_ES.md)

</div>

# LoRa-Based Crack Gauge (Fisurómetro) 🚀

This repository contains the code for a **LoRa-based crack gauge (fisurómetro)** project, which uses a **Black Pill v2.0 (STM32)** microcontroller to measure and transmit data on structural cracks through a LoRa network.

## Features

- **3D Printed Crack Gauge**: The sensor and structure are built using 3D-printed components, making it customizable for different environments.
- **Black Pill v2.0 (STM32)**: The fisurómetro is powered by an Black Pill v2.0 (STM32) microcontroller, allowing for flexible integration with various sensors.
- **LoRa Communication**: The device sends the sensor data via LoRa, a long-range communication protocol.
- **Power Supply**: It runs on 2S batteries, suitable for long-term, low-power applications.
- **Educational Outreach**: The project is designed with educational activities in mind, particularly for schools and universities.

## Hardware Requirements

- **Black Pill v2.0 (STM32)** microcontroller
- **LoRa Module** (e.g., RFM95W)
- **2S Batteries**
- **Crack Gauge Sensor** (Fisurómetro)
- Custom 3D-printed components for mounting

## Software Requirements

- **STM32CubeIDE** or any compatible platform for STM32 development
- Required libraries:
  - `LMIC`: For LoRa communication management
  - `Wire.h`: For I2C communication with sensors
  - Additional libraries as per your sensor configuration

## Installation

1. **Clone the repository**:

   Clone this repository to your local machine using:

   ```bash
   git clone https://github.com/MedialabUniovi/IOT_FissureMeter.git
   ```

2. **Set up the environment**:

   Open the project in **ArduinoIDE** and ensure you have installed the required libraries.

3. **Assemble the hardware**:

   Connect the Black Pill v2.0 board to your LoRa module, and attach the fisurómetro sensor. Power the setup using 2S batteries.

4. **Upload the code**:

   Compile and upload the code to the **Black Pill v2.0 (STM32)** microcontroller.

## Usage

1. **Power on the device**: 
   Once powered, the device will begin reading data from the fisurómetro sensor and transmitting it via LoRa.

2. **Data reception**:
   The transmitted data can be collected on the receiving end of the LoRa network for analysis.

3. **Customization**:
   Modify the code to suit your needs by adding additional sensors or improving the data logging system.

## Educational Activities 🌍

This project is intended for educational purposes and can be used to teach concepts such as **structural monitoring**, **IoT**, and **LoRa networks**. It's a great tool for engaging students in hands-on learning about the integration of hardware, coding, and communication technologies.

## About MediaLab Uniovi

<img src="img/medialab_logo.png" alt="MediaLab Uniovi Logo" width="200"/>

**MediaLab Uniovi** is a university laboratory of technology and design that was created in 2018 as a result of a collaboration between the **City Council of Gijón**, **Gijón Impulsa**, and the **University of Oviedo**. Its mission is to bring new technologies and project creation methodologies closer to people. MediaLab is based at the **School of Engineering of Gijón** and works to foster technological innovation and creativity.


## How to Contribute

Contributions are welcome! Whether you'd like to add new features, improve the code, or develop educational resources, feel free to submit a pull request.

## License

This project is licensed under the [MIT License](LICENSE).
