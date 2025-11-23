# WMNDLS - Wireless Mandala LED System

A project implementing wireless control and synchronization of WS2812B LED strips using the ESP-NOW protocol. The system consists of one transmitter (Server) and multiple receivers (Client).

## Project Description

The system allows central control of lighting effects on multiple remote devices without the need to connect them with wires or use an external Wi-Fi network. Communication occurs directly between ESP devices.

## Hardware Requirements

### Transmitter (Server)

- **Microcontroller:** Any ESP8266-based module (ESP32 can be used but libraries need to be adapted)
- **Role:** Transmits data to all receivers

### Receiver (Client)

- **Microcontroller:** ESP-01s (due to its small size)
- **Actuators:** Addressable LED strip (e.g., WS2812B)
- **Power Supply:** Appropriate for LED strip and ESP module (typically 5V with 3.3V regulator for ESP)

## Pin Configuration (ESP-01s Receiver)

In file `src/client/main_client.cpp`:

- **LED_PIN (GPIO 2):** LED strip data pin
- **BUTTON_PIN (GPIO 3 / RX):** Optional button (e.g., for changing device ID)

## Communication Protocol

Communication is based on the **ESP-NOW** protocol, which is a fast, connectionless radio communication protocol developed by Espressif. However, to overcome the 20 one-to-one connection limit, we forgo encryption - the transmitter uses a broadcast address.

### Data Packet Structure

Data is transmitted in the `structMessage` structure

| Field       | Type       | Description                                                                        |
| ----------- | ---------- | ---------------------------------------------------------------------------------- |
| `networkId` | `uint16_t` | Network identifier (`0x308B`). Protects against receiving data from other systems. |
| `deviceId`  | `uint8_t`  | Target device address. `0` means broadcast (to all).                               |
| `type`      | `uint8_t`  | Command type (e.g., `MSG_TYPE_UPDATE_LEDS`).                                       |
| `order`     | `uint32_t` | Sequential packet number. Used to ignore old or duplicate packets.                 |
| `ledCount`  | `uint16_t` | Number of LEDs in the packet.                                                      |
| `leds`      | `CRGB[]`   | Color array for each LED.                                                          |

### Protocol Operation

1.  **Transmission:** Server calculates animation frame, increments `order` counter and sends the entire `leds` color array to broadcast address.
2.  **Reception:** Client listens for packets. Upon receiving, it checks:

- Whether `networkId` matches.
- Whether packet is addressed to it or is a broadcast.
- Whether `order` is greater than last received (protection against packet ordering).

3.  **Display:** If verification is successful, client immediately displays the transmitted data.

## Directory Structure

- `src/client/` - Source code for receivers (ESP-01s).
- `src/server/` - Source code for transmitter.
- `src/shared/` - Shared header files and definitions (protocol).

## Applications

The project is ideal for:

- Decorative lighting distributed throughout a room.
- Art installations where multiple elements need to synchronize their lighting without cable clutter.
- Stage and event lighting.
