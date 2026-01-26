# AGENTS.md

## Project: Raspberry Pi Smart Blind Controller (Python)

### Core Directives
- **Architecture**: Domain Driven Design (DDD) with strict Separation of Concerns (SoC).
- **Platform**: Raspberry Pi (Python 3.9+).
- **Hardware**: GPIO for Stepper + Limits.

### Directory Structure
- `src/domain`: Pure Python logic. No hardware imports.
- `src/application`: Use cases, orchestration.
- `src/infrastructure`: Hardware drivers (RPi.GPIO), FastAPI, Paho MQTT.
- `src/interfaces`: Abstract Base Classes (ABCs).
- `main.py`: Entry point.

### Coding Standards
- Use Type Hints (`typing`).
- Use `pydantic` for data models if needed.
- AsyncIO for concurrency (FastAPI + Async MQTT wrapper if possible, or threaded).

### Hardware (Raspberry Pi)
- **Pins (BCM)**:
    - STEP: 17
    - DIR: 27
    - ENABLE: 22
    - LIMIT_TOP: 23
    - LIMIT_BOTTOM: 24
