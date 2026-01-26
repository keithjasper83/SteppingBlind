import logging
import time
import threading
from ..interfaces.hardware import IMotor, ILimitSwitch

logger = logging.getLogger(__name__)

class MockMotor(IMotor):
    def __init__(self, step_pin: int, dir_pin: int, enable_pin: int):
        self.step_pin = step_pin
        self.dir_pin = dir_pin
        self.enable_pin = enable_pin

        self._position = 0
        self._target_position = 0
        self._speed_delay = 0.001
        self._running = False
        self._stop_flag = False
        self._enabled = False

        logger.info(f"MockMotor initialized on pins STEP={step_pin}, DIR={dir_pin}, EN={enable_pin}")

    def set_speed(self, speed: int):
        if speed > 0:
            self._speed_delay = 1.0 / speed
        logger.debug(f"MockMotor speed set to {speed}")

    def set_acceleration(self, accel: int):
        logger.debug(f"MockMotor acceleration set to {accel}")

    def move(self, steps: int):
        self._target_position = self._position + steps
        logger.info(f"MockMotor moving {steps} steps. Target: {self._target_position}")
        self._start_motor_thread()

    def move_to(self, position: int):
        self._target_position = position
        logger.info(f"MockMotor moving to position {position}")
        self._start_motor_thread()

    def _start_motor_thread(self):
        if not self._running:
            t = threading.Thread(target=self._run_motor_loop, daemon=True)
            t.start()

    def _run_motor_loop(self):
        if not self._enabled:
            logger.warning("MockMotor is disabled, cannot move.")
            return

        self._running = True
        self._stop_flag = False

        logger.debug("MockMotor started moving...")

        while self._position != self._target_position and not self._stop_flag:
            direction = 1 if self._target_position > self._position else -1

            # Simulate delay
            time.sleep(self._speed_delay)

            self._position += direction

            # Log every 100 steps to avoid spam
            if self._position % 100 == 0:
                logger.debug(f"MockMotor pos: {self._position}")

        self._running = False
        logger.info(f"MockMotor stopped at {self._position}")

    def stop(self):
        self._stop_flag = True
        logger.info("MockMotor stop requested")

    def emergency_stop(self):
        self._stop_flag = True
        self.enable(False)
        logger.warning("MockMotor EMERGENCY STOP")

    def is_running(self) -> bool:
        return self._running

    def get_current_position(self) -> int:
        return self._position

    def set_current_position(self, position: int):
        self._position = position
        logger.info(f"MockMotor position reset to {position}")

    def enable(self, enabled: bool):
        self._enabled = enabled
        logger.info(f"MockMotor enabled: {enabled}")

class MockLimitSwitch(ILimitSwitch):
    def __init__(self, pin: int, triggered: bool = False):
        self.pin = pin
        self._triggered = triggered
        logger.info(f"MockLimitSwitch initialized on pin {pin}")

    def is_triggered(self) -> bool:
        return self._triggered

    def set_triggered(self, triggered: bool):
        self._triggered = triggered
        logger.info(f"MockLimitSwitch pin {self.pin} triggered: {triggered}")
