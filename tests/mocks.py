from src.interfaces.hardware import IMotor, ILimitSwitch, IStorage

class MockMotor(IMotor):
    def __init__(self):
        self.current_pos = 0
        self.target_pos = 0
        self.running = False
        self.enabled = False

    def set_speed(self, speed: int): pass
    def set_acceleration(self, accel: int): pass

    def move(self, steps: int):
        self.target_pos = self.current_pos + steps
        self.running = True

    def move_to(self, position: int):
        self.target_pos = position
        self.running = True

    def stop(self):
        self.running = False

    def emergency_stop(self):
        self.running = False

    def is_running(self) -> bool:
        return self.running

    def get_current_position(self) -> int:
        return self.current_pos

    def set_current_position(self, position: int):
        self.current_pos = position

    def enable(self, enabled: bool):
        self.enabled = enabled

    def complete_move(self):
        self.current_pos = self.target_pos
        self.running = False

class MockLimitSwitch(ILimitSwitch):
    def __init__(self):
        self.triggered = False

    def is_triggered(self) -> bool:
        return self.triggered

class MockStorage(IStorage):
    def __init__(self):
        self.saved_pos = -1

    def save_position(self, position: int):
        self.saved_pos = position

    def load_position(self) -> int:
        return self.saved_pos
