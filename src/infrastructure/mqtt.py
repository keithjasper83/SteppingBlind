import paho.mqtt.client as mqtt
from typing import Dict, List
from ..interfaces.network import IMqttClient, MqttCallback

class PahoMqttClient(IMqttClient):
    def __init__(self, host: str, port: int, client_id: str, username: str = None, password: str = None):
        self.client = mqtt.Client(client_id=client_id)
        if username and password:
            self.client.username_pw_set(username, password)

        self.host = host
        self.port = port
        self.subscriptions: Dict[str, List[MqttCallback]] = {}

        self.client.on_connect = self._on_connect
        self.client.on_message = self._on_message

    def start(self):
        self.client.connect_async(self.host, self.port, 60)
        self.client.loop_start()

    def publish(self, topic: str, payload: str, retain: bool = False):
        self.client.publish(topic, payload, retain=retain)

    def subscribe(self, topic: str, callback: MqttCallback):
        if topic not in self.subscriptions:
            self.subscriptions[topic] = []
        self.subscriptions[topic].append(callback)
        self.client.subscribe(topic)

    def _on_connect(self, client, userdata, flags, rc):
        print(f"Connected to MQTT broker with code {rc}")
        for topic in self.subscriptions:
            client.subscribe(topic)

    def _on_message(self, client, userdata, msg):
        topic = msg.topic
        payload = msg.payload.decode()

        if topic in self.subscriptions:
            for cb in self.subscriptions[topic]:
                cb(topic, payload)
