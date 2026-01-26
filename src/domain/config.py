from pydantic import BaseModel

class BlindConfig(BaseModel):
    max_travel_steps: int = 10000
    default_speed: int = 1000
    homing_speed: int = 500
    acceleration: int = 1000
    invert_direction: bool = False
    has_limit_switches: bool = False
