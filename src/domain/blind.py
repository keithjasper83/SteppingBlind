from typing import Optional
from ..interfaces.hardware import IMotor, ILimitSwitch, IStorage
from .config import BlindConfig
from .state import BlindState, HomingState

class Blind:
    def __init__(self, motor: IMotor, storage: IStorage, config: BlindConfig,
                 top_limit: Optional[ILimitSwitch] = None,
                 bottom_limit: Optional[ILimitSwitch] = None):
        self._motor = motor
        self._storage = storage
        self._config = config
        self._top_limit = top_limit
        self._bottom_limit = bottom_limit

        self._current_state = BlindState.UNKNOWN
        self._homing_state = HomingState.NONE

    @property
    def config(self) -> BlindConfig:
        return self._config

    def initialize(self):
        self._motor.set_speed(self._config.default_speed)
        self._motor.set_acceleration(self._config.acceleration)
        self._motor.enable(True)

        saved_pos = self._storage.load_position()
        if saved_pos is not None and 0 <= saved_pos <= self._config.max_travel_steps:
            self._motor.set_current_position(saved_pos)
            self.set_state(BlindState.IDLE)
        else:
            self.set_state(BlindState.UNKNOWN)

    def update(self):
        self.check_limits()

        # Homing State Machine
        if self._current_state == BlindState.HOMING:
            if self._homing_state == HomingState.MOVING_TO_BOTTOM:
                if self._bottom_limit and self._bottom_limit.is_triggered():
                    self._motor.stop()
                    self._motor.set_current_position(self._config.max_travel_steps)
                    self._homing_state = HomingState.RETRACTING
                    self._motor.move(-100) # Retract
            elif self._homing_state == HomingState.RETRACTING:
                if not self._motor.is_running():
                    self._homing_state = HomingState.DONE
                    self.set_state(BlindState.IDLE)
                    self._storage.save_position(self._config.max_travel_steps)
            return

        # Regular monitoring
        if not self._motor.is_running():
            if self._current_state in [BlindState.MOVING_UP, BlindState.MOVING_DOWN]:
                self.set_state(BlindState.IDLE)
                self._storage.save_position(self._motor.get_current_position())

    def check_limits(self):
        if self._homing_state != HomingState.NONE:
            return

        if self._top_limit and self._top_limit.is_triggered():
            if self._current_state == BlindState.MOVING_UP:
                self._motor.emergency_stop()
                self._motor.set_current_position(0)
                self.set_state(BlindState.IDLE)

        if self._bottom_limit and self._bottom_limit.is_triggered():
            if self._current_state == BlindState.MOVING_DOWN:
                self._motor.emergency_stop()
                self._motor.set_current_position(self._config.max_travel_steps)
                self.set_state(BlindState.IDLE)

    def move_up(self):
        if self._current_state in [BlindState.HOMING, BlindState.FAULT]:
            return

        if self._motor.get_current_position() <= 0 and not self._config.has_limit_switches:
            return

        self._motor.move_to(0)
        self.set_state(BlindState.MOVING_UP)

    def move_down(self):
        if self._current_state in [BlindState.HOMING, BlindState.FAULT]:
            return

        if self._motor.get_current_position() >= self._config.max_travel_steps and not self._config.has_limit_switches:
            return

        self._motor.move_to(self._config.max_travel_steps)
        self.set_state(BlindState.MOVING_DOWN)

    def move_to_percent(self, percent: int):
        if self._current_state in [BlindState.HOMING, BlindState.FAULT]:
            return

        percent = max(0, min(100, percent))
        target_steps = int((percent / 100.0) * self._config.max_travel_steps)

        current_steps = self._motor.get_current_position()
        if target_steps < current_steps:
            self.set_state(BlindState.MOVING_UP)
        elif target_steps > current_steps:
            self.set_state(BlindState.MOVING_DOWN)
        else:
            return

        self._motor.move_to(target_steps)

    def stop(self):
        self._motor.stop()
        self.set_state(BlindState.IDLE)

    def start_homing(self):
        if not self._config.has_limit_switches or not self._bottom_limit:
            return

        self.set_state(BlindState.HOMING)
        self._homing_state = HomingState.MOVING_TO_BOTTOM
        self._motor.set_speed(self._config.homing_speed)
        self._motor.move(100000) # Move down indefinitely

    def get_position_percent(self) -> int:
        if self._config.max_travel_steps == 0:
            return 0
        steps = self._motor.get_current_position()
        pc = (steps / self._config.max_travel_steps) * 100.0
        return max(0, min(100, int(pc)))

    def get_current_steps(self) -> int:
        return self._motor.get_current_position()

    def get_state(self) -> BlindState:
        return self._current_state

    def set_state(self, new_state: BlindState):
        self._current_state = new_state
