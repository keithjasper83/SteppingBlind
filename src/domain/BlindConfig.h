#pragma once
#include <cstdint>

namespace Blinds {
    namespace Domain {
        struct BlindConfig {
            int32_t maxTravelSteps = 10000;
            uint32_t defaultSpeed = 1000;
            uint32_t homingSpeed = 500;
            uint32_t acceleration = 1000;
            bool invertDirection = false;
            bool hasLimitSwitches = false;
        };
    }
}
