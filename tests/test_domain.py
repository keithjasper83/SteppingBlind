import pytest
from src.domain.blind import Blind, BlindConfig, BlindState
from tests.mocks import MockMotor, MockLimitSwitch, MockStorage

@pytest.fixture
def hardware():
    motor = MockMotor()
    top_limit = MockLimitSwitch()
    bottom_limit = MockLimitSwitch()
    storage = MockStorage()
    return motor, top_limit, bottom_limit, storage

@pytest.fixture
def blind(hardware):
    motor, top, bottom, storage = hardware
    config = BlindConfig(max_travel_steps=1000, has_limit_switches=True)
    return Blind(motor, storage, config, top, bottom)

def test_initialization_unknown(blind, hardware):
    _, _, _, storage = hardware
    storage.saved_pos = -1
    blind.initialize()
    assert blind.get_state() == BlindState.UNKNOWN

def test_initialization_restored(blind, hardware):
    motor, _, _, storage = hardware
    storage.saved_pos = 500
    blind.initialize()
    assert blind.get_state() == BlindState.IDLE
    assert motor.get_current_position() == 500

def test_move_up(blind, hardware):
    motor, _, _, storage = hardware
    storage.saved_pos = 500
    blind.initialize()

    blind.move_up()
    assert blind.get_state() == BlindState.MOVING_UP
    assert motor.target_pos == 0
    assert motor.running == True

def test_limit_switch_stop(blind, hardware):
    motor, top, _, storage = hardware
    storage.saved_pos = 500
    blind.initialize()
    blind.move_up()

    top.triggered = True
    blind.update()

    assert motor.running == False
    assert motor.current_pos == 0
    assert blind.get_state() == BlindState.IDLE

def test_homing(blind, hardware):
    motor, _, bottom, storage = hardware
    storage.saved_pos = -1
    blind.initialize()

    blind.start_homing()
    assert blind.get_state() == BlindState.HOMING
    assert motor.running == True

    # Trigger bottom limit
    bottom.triggered = True
    blind.update()

    # Should stop, set max pos, and retract
    assert motor.running == True # Retracting
    assert motor.current_pos == 1000

    # Finish retract
    motor.complete_move()
    blind.update()

    assert blind.get_state() == BlindState.IDLE
