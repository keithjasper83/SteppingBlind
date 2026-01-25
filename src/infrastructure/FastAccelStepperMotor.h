#pragma once
#include "../interfaces/HardwareInterfaces.h"
#include <FastAccelStepper.h>

namespace Blinds {
    namespace Infrastructure {
        class FastAccelStepperMotor : public Interfaces::IMotor {
        public:
            FastAccelStepperMotor(FastAccelStepperEngine& engine, uint8_t stepPin, uint8_t dirPin, uint8_t enablePin);

            void setSpeedInHz(uint32_t speed) override;
            void setAcceleration(uint32_t accel) override;
            void move(int32_t steps) override;
            void moveTo(int32_t position) override;
            void stop() override;
            void emergencyStop() override;
            bool isRunning() override;
            int32_t getCurrentPosition() override;
            void setCurrentPosition(int32_t position) override;
            void enable(bool enabled) override;

        private:
            FastAccelStepper* _stepper;
            uint8_t _enablePin;
        };
    }
}
