#pragma once
#include "../interfaces/HardwareInterfaces.h"
#include <Arduino.h>

namespace Blinds {
    namespace Infrastructure {
        class GpioLimitSwitch : public Interfaces::ILimitSwitch {
        public:
            GpioLimitSwitch(uint8_t pin, bool invert = true); // Default input_pullup -> low is triggered
            bool isTriggered() override;
        private:
            uint8_t _pin;
            bool _invert;
        };
    }
}
