#include "FastAccelStepperMotor.h"
#include <Arduino.h>

namespace Blinds {
    namespace Infrastructure {

        FastAccelStepperMotor::FastAccelStepperMotor(FastAccelStepperEngine& engine, uint8_t stepPin, uint8_t dirPin, uint8_t enablePin)
            : _enablePin(enablePin) {

            _stepper = engine.stepperConnectToPin(stepPin);
            if (_stepper) {
                _stepper->setDirectionPin(dirPin);
                _stepper->setEnablePin(enablePin);
                _stepper->setAutoEnable(false); // We control enable manually or via logic
                // Default settings
                _stepper->setSpeedInHz(1000);
                _stepper->setAcceleration(1000);
            }
        }

        void FastAccelStepperMotor::setSpeedInHz(uint32_t speed) {
            if (_stepper) _stepper->setSpeedInHz(speed);
        }

        void FastAccelStepperMotor::setAcceleration(uint32_t accel) {
            if (_stepper) _stepper->setAcceleration(accel);
        }

        void FastAccelStepperMotor::move(int32_t steps) {
            if (_stepper) _stepper->move(steps);
        }

        void FastAccelStepperMotor::moveTo(int32_t position) {
            if (_stepper) _stepper->moveTo(position);
        }

        void FastAccelStepperMotor::stop() {
            if (_stepper) _stepper->stopMove();
        }

        void FastAccelStepperMotor::emergencyStop() {
            if (_stepper) _stepper->forceStopAndNewPosition(_stepper->getCurrentPosition());
        }

        bool FastAccelStepperMotor::isRunning() {
            return _stepper ? _stepper->isRunning() : false;
        }

        int32_t FastAccelStepperMotor::getCurrentPosition() {
            return _stepper ? _stepper->getCurrentPosition() : 0;
        }

        void FastAccelStepperMotor::setCurrentPosition(int32_t position) {
            if (_stepper) _stepper->setCurrentPosition(position);
        }

        void FastAccelStepperMotor::enable(bool enabled) {
            if (_stepper) {
                if (enabled) _stepper->enableOutputs();
                else _stepper->disableOutputs();
            }
        }
    }
}
