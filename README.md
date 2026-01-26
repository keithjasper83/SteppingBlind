# Raspberry Pi Smart Blind Controller

A production-grade Python application for stepper-motor based smart blinds on Raspberry Pi.
Pivot from ESP32 based on user feedback.

## Features
- **Clean Architecture**: DDD (Domain-Driven Design) with SoC.
- **Web UI**: Mobile-friendly control panel using Tailwind CSS (FastAPI).
- **REST API**: Full control via HTTP JSON API.
- **MQTT**: Real-time status updates and control.
- **Persistence**: Remembers position after reboot (JSON file).
- **Smooth Motion**: Threaded stepper control.

## Hardware Setup (Raspberry Pi)

### Pinout (BCM Mode)
| Function | Pin (GPIO) | Notes |
|----------|------------|-------|
| STEP | 17 | Stepper Driver STEP |
| DIR | 27 | Stepper Driver DIR |
| ENABLE | 22 | Stepper Driver EN |
| LIMIT TOP | 23 | Optional N.O. Switch to GND (Pullup) |
| LIMIT BOTTOM | 24 | Optional N.O. Switch to GND (Pullup) |

### Motor Driver
- Compatible with A4988, DRV8825, TMC2209.
- Connect VMOT to 12V/24V PSU.
- Connect VDD to 3.3V (RPi 3.3V).
- Connect GND to RPi GND.

## Installation

### One-Line Install (Recommended)
Run this command on your Raspberry Pi to install and configure everything automatically:
```bash
bash <(curl -fsSL https://raw.githubusercontent.com/keithjasper83/SteppingBlind/main/install.sh)
```

### Manual Installation

1. **Clone Repository**:
   ```bash
   git clone https://github.com/keithjasper83/SteppingBlind.git blind-controller
   cd blind-controller
   ```

2. **Run Setup Script**:
   ```bash
   ./scripts/setup.sh
   ```
   *Note: This script handles system dependencies (git, python3-venv, etc) and creates the systemd service. If you prefer to set up manually, please refer to the content of `scripts/setup.sh`.*

3. **Manual Run (Dev Mode)**:
   ```bash
   pip3 install -r requirements.txt
   python3 main.py
   ```
   Access UI at `http://<rpi-ip>:8080`.

## Configuration

Edit `main.py` or use Environment Variables:
- `MQTT_HOST` (default: localhost)
- `MQTT_PORT` (default: 1883)
- `MQTT_CLIENT_ID`

## Systemd Service

To run on boot:
1. Edit `blind-controller.service` with correct path.
2. Copy to `/etc/systemd/system/`.
3. `sudo systemctl enable blind-controller`
4. `sudo systemctl start blind-controller`

## Testing

Run unit tests (Mocked hardware):
```bash
python3 -m pytest
```
