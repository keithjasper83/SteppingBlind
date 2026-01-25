#pragma once

namespace Blinds {
    namespace Domain {
        enum class BlindState {
            UNKNOWN,
            IDLE,
            MOVING_UP,
            MOVING_DOWN,
            HOMING,
            FAULT
        };
    }
}
