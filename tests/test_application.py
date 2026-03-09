import pytest
import json
from unittest.mock import MagicMock
from src.application.controller import BlindController
from src.domain.blind import Blind, BlindConfig, BlindState
from tests.mocks import MockMotor, MockStorage

@pytest.fixture
def controller():
    motor = MockMotor()
    storage = MockStorage()
    config = BlindConfig(max_travel_steps=1000, has_limit_switches=False)
    blind = Blind(motor, storage, config)
    mqtt = MagicMock()
    return BlindController(blind, mqtt), blind, mqtt

def test_publish_state_on_change(controller):
    ctrl, blind, mqtt = controller
    blind.initialize() # State becomes UNKNOWN because MockStorage.saved_pos is -1

    # First call should publish
    ctrl._publish_state_if_changed()
    assert mqtt.publish.call_count == 1

    # Second call without change should NOT publish
    mqtt.publish.reset_mock()
    ctrl._publish_state_if_changed()
    assert mqtt.publish.call_count == 0

    # Change state and call should publish
    blind.move_to_percent(50)
    ctrl._publish_state_if_changed()
    assert mqtt.publish.call_count == 1

    # Check payload
    args, kwargs = mqtt.publish.call_args
    payload = json.loads(args[1])
    # Position might not have updated yet in mock motor, but state should be MOVING_DOWN
    assert payload["state"] == "MOVING_DOWN"

def test_publish_state_content(controller):
    ctrl, blind, mqtt = controller
    blind.initialize()

    ctrl._publish_state_if_changed()
    args, kwargs = mqtt.publish.call_args
    payload = json.loads(args[1])

    assert "position" in payload
    assert "steps" in payload
    assert "state" in payload
    assert payload["state"] == "UNKNOWN"
