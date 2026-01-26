from abc import ABC, abstractmethod
from typing import Callable

MqttCallback = Callable[[str, str], None]

class IMqttClient(ABC):
    @abstractmethod
    def publish(self, topic: str, payload: str, retain: bool = False):
        pass

    @abstractmethod
    def subscribe(self, topic: str, callback: MqttCallback):
        pass

    @abstractmethod
    def start(self):
        pass
