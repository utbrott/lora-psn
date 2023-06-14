# LoRa Private Sensor Network

Embedded Systems IOT project that utilizes STM32 Nucleo and USI STM32 LoRa expansion to create a private sensor network.

Developed for Master's Thesis @ WUT Warsaw University of Technology.

## Features

- Slave modules refresh sensor readouts periodically,
- Slave boards store readouts in a buffer, for faster response handling,
- Slave responses to Master requests are handled via interrupts,
- Master board handles requests on a non-blocking timer,

## Built with

- STM32 L152 Nucleo-64 development board,
- USI STM32 Nucleo expansion board for LoRa,
- BME280 I2C/SPI breakout board sensors (SLAVE modules),

## File structure:

```
.
├── include
│   ├── bme280.h
│   ├── debug.h
│   ├── globals.h
│   ├── lora.h
│   ├── main.h
│   └── rollingavg.h
├── src
│   ├── bme280.cpp
│   ├── debug.cpp
│   ├── lora.cpp
│   ├── main.cpp
│   └── rollingavg.cpp
├── platformio.ini
└── README.md   --> This file
```

- `bme280.h` - interfacing with Adafruit BME280 sensor breakout board,
- `debug.h` - helper functions for debugging through Serial,
- `globals.h` - project-wide definitions, typedefs, functions,
- `lora.h` - interfacing with USI STM32 LoRa expansion shield,

## Future development note

Project was built using PlatformIO Core CLI and PlatformIO IDE for VSCode.
