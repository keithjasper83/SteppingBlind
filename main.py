import uvicorn
import os
import signal
import sys
import logging
from src.domain.blind import Blind
from src.domain.config import BlindConfig
from src.infrastructure.hardware import RPiMotor, RPiLimitSwitch
from src.infrastructure.mock_hardware import SimulatedMotor, SimulatedLimitSwitch
from src.infrastructure.storage import FileStorage
from src.infrastructure.mqtt import PahoMqttClient
from src.application.controller import BlindController
from src.infrastructure.web import create_app

# Config
# Ideally load from env or config file
MQTT_HOST = os.getenv("MQTT_HOST", "localhost")
MQTT_PORT = int(os.getenv("MQTT_PORT", 1883))
MQTT_CLIENT_ID = os.getenv("MQTT_CLIENT_ID", "rpi-blinds")

# Pins (BCM)
PIN_STEP = 17
PIN_DIR = 27
PIN_ENABLE = 22
PIN_LIMIT_TOP = 23
PIN_LIMIT_BOTTOM = 24

def main():
    # Setup basic logging
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        handlers=[
            logging.StreamHandler(sys.stdout)
        ]
    )
    logger = logging.getLogger("main")

    logger.info("Starting Raspberry Pi Smart Blind Controller...")

    # Infrastructure
    motor = None
    limit_top = None
    limit_bottom = None

    try:
        logger.info("Attempting to initialize RPi Hardware...")
        motor = RPiMotor(PIN_STEP, PIN_DIR, PIN_ENABLE)
        # limit_top = RPiLimitSwitch(PIN_LIMIT_TOP)
        # limit_bottom = RPiLimitSwitch(PIN_LIMIT_BOTTOM)
        logger.info("RPi Hardware initialized successfully.")
    except (RuntimeError, ImportError) as e:
        logger.warning(f"Failed to initialize RPi Hardware: {e}")
        logger.warning("Falling back to Simulated Hardware (Simulation Mode).")
        try:
            motor = SimulatedMotor(PIN_STEP, PIN_DIR, PIN_ENABLE)
        except Exception as ex:
            logger.error(f"Failed to initialize Simulated Hardware: {ex}")

    if motor is None:
        logger.error("Failed to initialize both RPi and Simulated motor hardware. Exiting.")
        sys.exit(1)

    storage = FileStorage("blind_data.json")
    mqtt = PahoMqttClient(MQTT_HOST, MQTT_PORT, MQTT_CLIENT_ID)

    # Domain
    config = BlindConfig(
        max_travel_steps=20000,
        default_speed=1000,
        has_limit_switches=False
    )
    blind = Blind(motor, storage, config, limit_top, limit_bottom)

    # Application
    controller = BlindController(blind, mqtt)

    # Start Logic
    controller.start()

    # Web App
    app = create_app(controller)

    # Signal Handling
    def signal_handler(sig, frame):
        logger.info("Shutting down...")
        controller.stop()
        sys.exit(0)

    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)

    # Run Server
    logger.info("Starting Web Server on port 8080...")
    uvicorn.run(app, host="0.0.0.0", port=8080, log_level="info")

if __name__ == "__main__":
    main()
