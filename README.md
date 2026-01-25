# ESP32 Smart Blind Controller

A production-grade, Domain-Driven Design (DDD) firmware for stepper-motor based smart blinds on ESP32-S3.

## Features
- **Clean Architecture**: Strict Separation of Concerns (Domain, Application, Infrastructure).
- **Smooth Motion**: Uses `FastAccelStepper` for non-blocking, smooth acceleration.
- **Web UI**: Mobile-friendly control panel using Tailwind CSS (CDN).
- **REST API**: Full control via HTTP JSON API (compatible with Home Assistant / Jarvis).
- **MQTT**: Real-time status updates and control.
- **Persistence**: Remembers position after reboot (NVS).
- **Safety**: Limit switch support, soft limits, and homing.

## Hardware Setup (ESP32-S3)

### Pinout
| Function | Pin (GPIO) | Notes |
|----------|------------|-------|
| STEP | 4 | Stepper Driver STEP |
| DIR | 5 | Stepper Driver DIR |
| ENABLE | 6 | Stepper Driver EN |
| LIMIT TOP | 7 | N.O. Switch to GND (Pullup) |
| LIMIT BOTTOM | 15 | N.O. Switch to GND (Pullup) |
| BTN UP | 16 | Optional Pushbutton to GND |
| BTN DOWN | 17 | Optional Pushbutton to GND |

### Motor Driver
- Compatible with A4988, DRV8825, TMC2209 (Standalone/DIR/STEP).
- Connect VMOT to 12V/24V PSU.
- Connect VDD to 3.3V/5V (ESP32 Logic).
- Connect GND to ESP32 GND.

## Configuration

Open `src/main.cpp` to configure:
- **WiFi**: Update `WIFI_SSID` and `WIFI_PASS`.
- **MQTT**: Update `MQTT_HOST`, `PORT`, `CLIENT_ID`.
- **Calibration**: Adjust `config.maxTravelSteps` in `setup()` to match your blind's length.

## API Documentation

### HTTP REST
- **GET /health**: Returns `{"status":"ok"}`
- **GET /state**: Returns status JSON.
  ```json
  {
    "position": 50,
    "steps": 10000,
    "state": "IDLE"
  }
  ```
- **POST /move**: `{"cmd": "up" | "down" | "stop"}`
- **POST /goto**: `{"percent": 0-100}`
- **POST /home**: Triggers homing sequence.

### MQTT
- **Subscribe**: `blind/state` (JSON status updates)
- **Publish**: `blind/cmd` (Payload: `UP`, `DOWN`, `STOP`, `HOME`)
- **Publish**: `blind/position/set` (Payload: `0`-`100`)
- **Publish**: `blind/set` (JSON: `{"cmd":"up"}` or `{"position":50}`)

## Build & Flash

Requirements:
- PlatformIO (VSCode Extension or CLI)

1. Clone repo.
2. `pio run` to build.
3. Connect ESP32-S3 via USB.
4. `pio run -t upload` to flash.
5. `pio run -t monitor` to view serial logs.

## Testing

Run unit tests (Domain logic):
```bash
pio test -e native
```
