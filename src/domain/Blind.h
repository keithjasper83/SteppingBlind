#pragma once

#include <functional>
#include <string>
#include "../interfaces/HardwareInterfaces.h"
#include "BlindConfig.h"
#include "BlindState.h"

namespace Blinds {
    namespace Domain {

        class Blind {
        public:
            Blind(Interfaces::IMotor& motor,
                  Interfaces::ILimitSwitch* topLimit,
                  Interfaces::ILimitSwitch* bottomLimit,
                  Interfaces::IStorage& storage,
                  BlindConfig config);

            void initialize();
            void update(); // Call this in main loop

            // Commands
            void moveUp();
            void moveDown();
            void stop();
            void moveToPercent(int percent);
            void startHoming();

            // Status
            int getPositionPercent();
            int32_t getCurrentSteps();
            BlindState getState();
            std::string getStateString();

        private:
            Interfaces::IMotor& _motor;
            Interfaces::ILimitSwitch* _topLimit;
            Interfaces::ILimitSwitch* _bottomLimit;
            Interfaces::IStorage& _storage;
            BlindConfig _config;

            BlindState _currentState;
            int32_t _targetSteps;

            // Homing sub-state
            enum class HomingState {
                NONE,
                MOVING_TO_BOTTOM,
                RETRACTING,
                DONE
            };
            HomingState _homingState = HomingState::NONE;

            void setState(BlindState newState);
            void checkLimits();
            int32_t percentToSteps(int percent);
            int stepsToPercent(int32_t steps);
        };
    }
}
