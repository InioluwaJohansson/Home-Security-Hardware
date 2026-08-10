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
| 4 | Door 0 encoder A | Digital input |
| 5 | Door 0 encoder B | Digital input |
| 12 | Door 1 encoder A | Digital input (ESP32 strap pin) |
| 13 | Door 1 encoder B | Digital input |
| 15 | Door 2 encoder A | Digital input (ESP32 strap pin) |
| 18 | Door 2 encoder B | Digital input |
| 19 | Door 3 encoder A | Digital input |
| 2 | Door 3 encoder B | Digital input |
| 23 | Reset button (10-second hold) | Digital input, internal pull-up |

PN532/NFID is **I2C**, not SPI. It shares GPIO 21/22 with the MCP23017,
PCA9685 and ADS1115 devices. The old RFID SPI allocation is intentionally
removed because this project uses the PN532 I2C constructor.

## MCP23017

Four expanders are used at `0x20`-`0x23`.

- `0x20`: lights, appliances, cameras 0-1
- `0x21`: camera 2, door movement direction, door lock direction
- `0x22`: window movement direction, window lock direction, external outputs, buzzer, siren
- `0x23`: window encoder A/B inputs and spare encoder inputs

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

- GP0-GP7: window encoder A/B, windows 0-3
- GP8-GP15: reserved

Door encoder A/B inputs use dedicated ESP32 GPIOs and are not multiplexed.
Window encoder inputs on the MCP23017 are reserved for telemetry/diagnostics;
they are not the primary safety stop. The actual safety stop remains the
open/close Hall sensor, lock-position sensor, current limit, and maximum
travel time.

GPIO 2, 12 and 15 are ESP32 strapping pins. The encoder interface must not
force an invalid boot level; use an encoder electrical interface with
appropriate external biasing.

## PCA9685

Two controllers are used.

### `0x40` — motors and locks

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

Each door uses three channels:

- `door * 3 + 0`: fully-open Hall sensor
- `door * 3 + 1`: fully-closed Hall sensor
- `door * 3 + 2`: lock/unlock position sensor

Four doors use channels 0-11.

### Window multiplexer

Each window uses three channels:

- `window * 3 + 0`: fully-open Hall sensor
- `window * 3 + 1`: fully-closed Hall sensor
- `window * 3 + 2`: lock/unlock position sensor

Four windows use channels 0-11.

### External multiplexer

- CH0-CH4: five EKMC1601111 PIR external inputs
- CH5-CH15: reserved

No facility attempts to access a channel above 15.

All five external PIR inputs are active-high digital inputs. `IsActive=false`
means the controller does not poll that PIR.

## ADS1115

Four ADCs are used at `0x48`-`0x4B`.

- `0x48`: door movement current, channels 0-3
- `0x49`: window movement current, channels 0-3
- `0x4A`: door lock current, channels 0-3
- `0x4B`: window lock current, channels 0-3

Current sensor zero voltage and sensitivity remain calibration values and must
be matched to the actual Hall current sensor used with the motor driver.
