import time
import RPi.GPIO as GPIO
from ..interfaces.hardware import IMotor, ILimitSwitch

class RPiMotor(IMotor):
    def __init__(self, step_pin: int, dir_pin: int, enable_pin: int):
        self.step_pin = step_pin
        self.dir_pin = dir_pin
        self.enable_pin = enable_pin

        GPIO.setmode(GPIO.BCM)
        GPIO.setup(self.step_pin, GPIO.OUT)
        GPIO.setup(self.dir_pin, GPIO.OUT)
        GPIO.setup(self.enable_pin, GPIO.OUT)

        self._position = 0
        self._target_position = 0
        self._speed_delay = 0.001 # Default
        self._running = False
        self._stop_flag = False
        self._enabled = False

    def set_speed(self, speed: int):
        # Speed in Hz (steps per second) -> delay = 1/speed
        if speed > 0:
            self._speed_delay = 1.0 / speed

    def set_acceleration(self, accel: int):
        # Simple implementation ignores accel for now, just jumps to speed
        pass

    def move(self, steps: int):
        self._target_position = self._position + steps
        self._start_motor_thread()

    def move_to(self, position: int):
        self._target_position = position
        self._start_motor_thread()

    def _start_motor_thread(self):
        if not self._running:
            import threading
            t = threading.Thread(target=self._run_motor_loop, daemon=True)
            t.start()

    def _run_motor_loop(self):
        if not self._enabled:
            return

        self._running = True
        self._stop_flag = False

        while self._position != self._target_position and not self._stop_flag:
            direction = 1 if self._target_position > self._position else -1
            GPIO.output(self.dir_pin, GPIO.HIGH if direction > 0 else GPIO.LOW)

            GPIO.output(self.step_pin, GPIO.HIGH)
            time.sleep(self._speed_delay / 2)
            GPIO.output(self.step_pin, GPIO.LOW)
            time.sleep(self._speed_delay / 2)
            self._position += direction

        self._running = False

    def stop(self):
        self._stop_flag = True

    def emergency_stop(self):
        self._stop_flag = True
        self.enable(False)

    def is_running(self) -> bool:
        return self._running

    def get_current_position(self) -> int:
        return self._position

    def set_current_position(self, position: int):
        self._position = position

    def enable(self, enabled: bool):
        self._enabled = enabled
        # Active LOW enable usually
        GPIO.output(self.enable_pin, GPIO.LOW if enabled else GPIO.HIGH)

class RPiLimitSwitch(ILimitSwitch):
    def __init__(self, pin: int, invert: bool = True):
        self.pin = pin
        self.invert = invert
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(self.pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

    def is_triggered(self) -> bool:
        state = GPIO.input(self.pin)
        return not state if self.invert else state
