from enum import Enum

class BlindState(str, Enum):
    UNKNOWN = "UNKNOWN"
    IDLE = "IDLE"
    MOVING_UP = "MOVING_UP"
    MOVING_DOWN = "MOVING_DOWN"
    HOMING = "HOMING"
    FAULT = "FAULT"

class HomingState(str, Enum):
    NONE = "NONE"
    MOVING_TO_BOTTOM = "MOVING_TO_BOTTOM"
    RETRACTING = "RETRACTING"
    DONE = "DONE"
