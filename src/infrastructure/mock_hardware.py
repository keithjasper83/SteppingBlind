import logging
import time
import threading
from ..interfaces.hardware import IMotor, ILimitSwitch

logger = logging.getLogger(__name__)

class SimulatedMotor(IMotor):
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
        self._lock = threading.Lock()
        self._thread = None

        logger.info(f"SimulatedMotor initialized on pins STEP={step_pin}, DIR={dir_pin}, EN={enable_pin}")

    def set_speed(self, speed: int):
        if speed <= 0:
            logger.warning(f"SimulatedMotor set_speed called with non-positive speed: {speed}")
            raise ValueError(f"speed must be a positive integer, got {speed}")

        with self._lock:
            self._speed_delay = 1.0 / speed
        logger.debug(f"SimulatedMotor speed set to {speed}")

    def set_acceleration(self, accel: int):
        logger.debug(f"SimulatedMotor acceleration set to {accel}")

    def move(self, steps: int):
        with self._lock:
            self._target_position = self._position + steps
            logger.info(f"SimulatedMotor moving {steps} steps. Target: {self._target_position}")
        self._start_motor_thread()

    def move_to(self, position: int):
        with self._lock:
            self._target_position = position
            logger.info(f"SimulatedMotor moving to position {position}")
        self._start_motor_thread()

    def _start_motor_thread(self):
        with self._lock:
            # If thread is already running, it will pick up the new target position
            if self._thread is not None and self._thread.is_alive():
                logger.debug("SimulatedMotor thread already running, updating target position")
                return

            # Start new thread
            self._thread = threading.Thread(target=self._run_motor_loop, daemon=True)
            self._thread.start()

    def _run_motor_loop(self):
        with self._lock:
            if not self._enabled:
                logger.warning("SimulatedMotor is disabled, cannot move.")
                return
            self._running = True
            self._stop_flag = False

        logger.debug("SimulatedMotor started moving...")

        try:
            while True:
                with self._lock:
                    if self._stop_flag or self._position == self._target_position:
                        break

                    direction = 1 if self._target_position > self._position else -1
                    current_delay = self._speed_delay

                # Simulate delay (outside lock)
                time.sleep(current_delay)

                with self._lock:
                    self._position += direction
                    current_pos = self._position

                # Log every 100 steps to avoid spam
                if current_pos % 100 == 0:
                    logger.debug(f"SimulatedMotor pos: {current_pos}")

        finally:
            with self._lock:
                self._running = False
            logger.info(f"SimulatedMotor stopped at {self._position}")

    def stop(self):
        with self._lock:
            self._stop_flag = True
        logger.info("SimulatedMotor stop requested")

    def emergency_stop(self):
        with self._lock:
            self._stop_flag = True
        self.enable(False)
        logger.warning("SimulatedMotor EMERGENCY STOP")

    def is_running(self) -> bool:
        with self._lock:
            return self._running

    def get_current_position(self) -> int:
        with self._lock:
            return self._position

    def set_current_position(self, position: int):
        with self._lock:
            self._position = position
        logger.info(f"SimulatedMotor position reset to {position}")

    def enable(self, enabled: bool):
        with self._lock:
            self._enabled = enabled
        logger.info(f"SimulatedMotor enabled: {enabled}")

class SimulatedLimitSwitch(ILimitSwitch):
    def __init__(self, pin: int, triggered: bool = False):
        self.pin = pin
        self._triggered = triggered
        logger.info(f"SimulatedLimitSwitch initialized on pin {pin}")

    def is_triggered(self) -> bool:
        return self._triggered

    def set_triggered(self, triggered: bool):
        self._triggered = triggered
        logger.info(f"SimulatedLimitSwitch pin {self.pin} triggered: {triggered}")
