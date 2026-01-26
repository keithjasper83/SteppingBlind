from abc import ABC, abstractmethod
from typing import Callable

class IMotor(ABC):
    @abstractmethod
    def set_speed(self, speed: int):
        pass

    @abstractmethod
    def set_acceleration(self, accel: int):
        pass

    @abstractmethod
    def move(self, steps: int):
        pass

    @abstractmethod
    def move_to(self, position: int):
        pass

    @abstractmethod
    def stop(self):
        pass

    @abstractmethod
    def emergency_stop(self):
        pass

    @abstractmethod
    def is_running(self) -> bool:
        pass

    @abstractmethod
    def get_current_position(self) -> int:
        pass

    @abstractmethod
    def set_current_position(self, position: int):
        pass

    @abstractmethod
    def enable(self, enabled: bool):
        pass

class ILimitSwitch(ABC):
    @abstractmethod
    def is_triggered(self) -> bool:
        pass

class IStorage(ABC):
    @abstractmethod
    def save_position(self, position: int):
        pass

    @abstractmethod
    def load_position(self) -> int:
        pass
