# AGENTS.md

## Project: ESP32 Smart Blind Controller (DDD/SoC)

### Core Directives
- **Architecture**: Domain Driven Design (DDD) with strict Separation of Concerns (SoC).
- **Platform**: ESP32-S3 (Arduino framework via PlatformIO).
- **Language**: C++17.

### Directory Structure
- `src/domain`: Pure C++ logic. NO Arduino includes. Testable.
- `src/application`: Use cases, orchestration.
- `src/infrastructure`: Hardware drivers, Wi-Fi, Web Server, MQTT.
- `src/interfaces`: DTOs, Abstract Interfaces.
- `src/main.cpp`: Entry point, DI wiring.

### Coding Standards
- Use `pragma once` in headers.
- Use `namespace` (e.g., `Blinds::Domain`, `Blinds::Infra`).
- No global variables in Domain logic.
- Use Dependency Injection (via constructor).
- All floating point logic should be double-checked (prefer integer steps).

### Testing
- Unit tests must be in `test/`.
- Domain logic must be unit-testable without hardware.

### Hardware (ESP32-S3-N16E8)
- **Flash**: 16MB
- **PSRAM**: 8MB (Octal)
- **Pins**:
    - STEP: GPIO 4
    - DIR: GPIO 5
    - ENABLE: GPIO 6
    - LIMIT_TOP: GPIO 7
    - LIMIT_BOTTOM: GPIO 15
    - BTN_UP: GPIO 16
    - BTN_DOWN: GPIO 17

### Functional Requirements
- Wi-Fi + MQTT + REST API + Web UI (Tailwind).
- Smooth stepper control.
- Safety limits.
