import json
import time
import threading
from typing import Dict, Any
from ..domain.blind import Blind, BlindState
from ..interfaces.network import IMqttClient

class BlindController:
    def __init__(self, blind: Blind, mqtt_client: IMqttClient):
        self.blind = blind
        self.mqtt = mqtt_client
        self._stop_event = threading.Event()
        self._thread = threading.Thread(target=self._loop, daemon=True)
        self._last_state_json = ""

    def start(self):
        self.blind.initialize()
        self.mqtt.start()

        # MQTT Subscriptions
        self.mqtt.subscribe("blind/cmd", self._on_mqtt_cmd)
        self.mqtt.subscribe("blind/position/set", self._on_mqtt_position)
        self.mqtt.subscribe("blind/set", self._on_mqtt_set)

        self._thread.start()

    def stop(self):
        self._stop_event.set()

    def _loop(self):
        while not self._stop_event.is_set():
            self.blind.update()
            self._publish_state_if_changed()
            time.sleep(0.01) # 10ms poll for limits responsiveness

    def _publish_state_if_changed(self):
        state = self.get_state_dict()
        state_json = json.dumps(state)

        if state_json != self._last_state_json:
            self.mqtt.publish("blind/state", state_json, retain=True)
            self._last_state_json = state_json

    def get_state_dict(self) -> Dict[str, Any]:
        steps = self.blind.get_current_steps()
        max_steps = self.blind.config.max_travel_steps

        position = 0
        if max_steps > 0:
            pc = (steps / max_steps) * 100.0
            position = max(0, min(100, int(pc)))

        return {
            "position": position,
            "steps": steps,
            "state": self.blind.get_state().value
        }

    # Use Cases
    def move_up(self):
        self.blind.move_up()

    def move_down(self):
        self.blind.move_down()

    def stop_blind(self):
        self.blind.stop()

    def move_to(self, percent: int):
        self.blind.move_to_percent(percent)

    def home(self):
        self.blind.start_homing()

    # MQTT Callbacks
    def _on_mqtt_cmd(self, topic: str, payload: str):
        cmd = payload.upper()
        if cmd == "UP": self.move_up()
        elif cmd == "DOWN": self.move_down()
        elif cmd == "STOP": self.stop_blind()
        elif cmd == "HOME": self.home()

    def _on_mqtt_position(self, topic: str, payload: str):
        try:
            percent = int(payload)
            self.move_to(percent)
        except ValueError:
            pass

    def _on_mqtt_set(self, topic: str, payload: str):
        try:
            data = json.loads(payload)
            if "cmd" in data:
                cmd = data["cmd"].lower()
                if cmd == "up": self.move_up()
                elif cmd == "down": self.move_down()
                elif cmd == "stop": self.stop_blind()
                elif cmd == "home": self.home()
            if "position" in data:
                self.move_to(int(data["position"]))
        except json.JSONDecodeError:
            pass
