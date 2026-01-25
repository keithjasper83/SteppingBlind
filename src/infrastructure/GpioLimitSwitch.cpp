#include "GpioLimitSwitch.h"

namespace Blinds {
    namespace Infrastructure {

        GpioLimitSwitch::GpioLimitSwitch(uint8_t pin, bool invert) : _pin(pin), _invert(invert) {
            pinMode(_pin, INPUT_PULLUP);
        }

        bool GpioLimitSwitch::isTriggered() {
            bool state = digitalRead(_pin);
            // If invert (pullup), LOW means triggered (true)
            return _invert ? !state : state;
        }
    }
}
