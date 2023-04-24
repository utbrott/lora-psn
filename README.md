# LoRaWAN Private Sensor Network

Embedded Systems IOT project that utilizes STM32 Nucleo and USI STM32 LoRa expansion to create a private sensor network.

Developed for Master's Thesis @ WUT Warsaw University of Technology.

# Features

- BME280 Sensor to read surrouding's general temperature and pressure
- Request to measuring SLAVE is sent only when button on MASTER board is pressed,

# Built with

- STM32 L152 Nucleo-64 development board,
- USI STM32 Nucleo expansion board for LoRa,
- BME280 I2C/SPI breakout board sensors,

# File structure:

```
|
|-- src
|   |
|   |- main.h
|   |- main.cpp
|   |- lora.h (interfacing with LoRa WAN Shield)
|   |- lora.cpp
|   |- bme280_sensor.h (interfacing, using Adafruit BME280 sensor)
|   |- bme280_sensor.cpp
|
|- platformio.ini (project config for PlatformIO)
|- README.md --> This file
```

## Future development note

Project was built using PlatformIO extension for VSCode.
