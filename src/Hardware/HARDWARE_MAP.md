# Hardware Resource Map

This map is the physical resource allocation used by the Hardware layer.

## ESP32 direct pins

| GPIO | Function | Type |
|---:|---|---|
| 21 | I2C SDA | I2C |
| 22 | I2C SCL | I2C |
| 25 | 4067 S0 | Digital output |
| 26 | 4067 S1 | Digital output |
| 27 | 4067 S2 | Digital output |
| 14 | 4067 S3 | Digital output |
| 34 | Door 4067 SIG | Input-only |
| 35 | Window 4067 SIG | Input-only |
| 36 | External 4067 SIG | Input-only |
| 16 | Fingerprint RX | UART2 |
| 17 | Fingerprint TX | UART2 |
| 18 | NFID SPI SCK | SPI |
| 19 | NFID SPI MISO | SPI |
| 23 | NFID SPI MOSI | SPI |
| 5 | NFID SPI SS | SPI |
| 4 | NFID reset | Digital output |
| 10 | Reset button | Digital input |

## MCP23017

Four expanders are used.

- `0x20`: lights, appliances, cameras 0-1
- `0x21`: camera 2, door movement direction, door lock direction
- `0x22`: window movement direction, window lock direction, external outputs, buzzer, siren
- `0x23`: encoder A/B inputs for all doors and windows

### MCP 0x20

- GP0-GP5: 6 lights
- GP6-GP13: 8 appliances
- GP14-GP15: cameras 0-1

### MCP 0x21

- GP0: camera 2
- GP1-GP4: door movement direction 0-3
- GP5-GP8: door lock direction 0-3
- GP9-GP15: reserved

### MCP 0x22

- GP0-GP3: window movement direction 0-3
- GP4-GP7: window lock direction 0-3
- GP8-GP12: external outputs 0-4
- GP13: buzzer
- GP14: siren
- GP15: reserved

### MCP 0x23

- GP0-GP7: door encoder A/B, doors 0-3
- GP8-GP15: window encoder A/B, windows 0-3

## PCA9685

Two controllers are used.

### `0x40` — motors

- CH0-CH3: door movement PWM 0-3
- CH4-CH7: door lock PWM 0-3
- CH8-CH11: window movement PWM 0-3
- CH12-CH15: window lock PWM 0-3

### `0x41` — lights

- CH0-CH5: light brightness PWM 0-5
- CH6-CH15: reserved

## 74HC4067

Three multiplexers share S0-S3 but have independent SIG inputs.

### Door multiplexer

12 used channels: 4 doors × (open Hall, close Hall, lock sensor). Channels 0-1, 5-6, 10-11, and 15 are reserved for additional sensors.

### Window multiplexer

12 used channels: 4 windows × (open Hall, close Hall, lock sensor). Channels 0-1, 5-6, 10-11, and 15 are reserved for additional sensors.

### External multiplexer

- CH8-CH12: 5 external inputs
- CH0-CH7 and CH13-CH15: reserved

Encoder signals are **not** multiplexed because they are higher-frequency signals.

## ADS1115

Four ADCs are used at `0x48`-`0x4B`.

- `0x48`: door movement current, channels 0-3
- `0x49`: window movement current, channels 0-3
- `0x4A`: door lock current, channels 0-3
- `0x4B`: window lock current, channels 0-3

Current sensor zero voltage and sensitivity are hardware-calibration values and must be changed to match the actual current sensor selected for the motors.
