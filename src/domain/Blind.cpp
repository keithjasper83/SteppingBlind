#include "Blind.h"
#include <algorithm>
#include <cmath>

namespace Blinds {
    namespace Domain {

        Blind::Blind(Interfaces::IMotor& motor,
                     Interfaces::ILimitSwitch* topLimit,
                     Interfaces::ILimitSwitch* bottomLimit,
                     Interfaces::IStorage& storage,
                     BlindConfig config)
            : _motor(motor),
              _topLimit(topLimit),
              _bottomLimit(bottomLimit),
              _storage(storage),
              _config(config),
              _currentState(BlindState::UNKNOWN)
        {
        }

        void Blind::initialize() {
            _motor.setSpeedInHz(_config.defaultSpeed);
            _motor.setAcceleration(_config.acceleration);
            _motor.enable(true);

            int32_t savedPos = _storage.loadPosition();
            // Simple validation of saved position
            if (savedPos >= 0 && savedPos <= _config.maxTravelSteps) {
                 _motor.setCurrentPosition(savedPos);
                 setState(BlindState::IDLE);
            } else {
                setState(BlindState::UNKNOWN);
                // If we have limits, we should probably home.
                // But we wait for explicit home command or startHoming() call.
            }
        }

        void Blind::update() {
            checkLimits();

            // State Machine for Homing
            if (_currentState == BlindState::HOMING) {
                if (_homingState == HomingState::MOVING_TO_BOTTOM) {
                    if (_bottomLimit && _bottomLimit->isTriggered()) {
                        _motor.stop();
                        _motor.setCurrentPosition(_config.maxTravelSteps);
                        _homingState = HomingState::RETRACTING;
                        // Retract slightly to clear switch
                        _motor.move(-100);
                    } else if (!_motor.isRunning()) {
                        // We stopped but didn't hit limit? Maybe stalled or manually stopped.
                         // For now, if we reach here and motor stopped, fail?
                         // Or if we commanded a huge move and it finished (unlikely if limit switch works)
                    }
                } else if (_homingState == HomingState::RETRACTING) {
                    if (!_motor.isRunning()) {
                        _homingState = HomingState::DONE;
                        setState(BlindState::IDLE);
                        _storage.savePosition(_config.maxTravelSteps);
                    }
                }
                return;
            }

            // Normal operation monitoring
            if (_motor.isRunning()) {
                // Direction detection?
                // For now, just rely on state set by commands, or infer from velocity if available.
                // But we need to know if we are moving UP or DOWN to set state correctly if we just check isRunning.
                // However, commands set the state.
            } else {
                if (_currentState == BlindState::MOVING_UP || _currentState == BlindState::MOVING_DOWN) {
                    setState(BlindState::IDLE);
                    _storage.savePosition(_motor.getCurrentPosition());
                }
            }
        }

        void Blind::checkLimits() {
            if (_homingState != HomingState::NONE) return; // Homing handles its own limits

            if (_topLimit && _topLimit->isTriggered()) {
                // If moving up, stop
                // We don't easily know current direction from IMotor without polling velocity,
                // but we can assume if we are at limit, we should hard stop and set pos.
                if (_currentState == BlindState::MOVING_UP) {
                    _motor.emergencyStop();
                    _motor.setCurrentPosition(0);
                    setState(BlindState::IDLE);
                }
            }

            if (_bottomLimit && _bottomLimit->isTriggered()) {
                if (_currentState == BlindState::MOVING_DOWN) {
                    _motor.emergencyStop();
                    _motor.setCurrentPosition(_config.maxTravelSteps);
                    setState(BlindState::IDLE);
                }
            }
        }

        void Blind::moveUp() {
            if (_currentState == BlindState::HOMING || _currentState == BlindState::FAULT) return;

            // Soft limit
            if (_motor.getCurrentPosition() <= 0 && !_config.hasLimitSwitches) {
                return;
            }

            _motor.moveTo(0);
            setState(BlindState::MOVING_UP);
        }

        void Blind::moveDown() {
            if (_currentState == BlindState::HOMING || _currentState == BlindState::FAULT) return;

            // Soft limit
            if (_motor.getCurrentPosition() >= _config.maxTravelSteps && !_config.hasLimitSwitches) {
                return;
            }

            _motor.moveTo(_config.maxTravelSteps);
            setState(BlindState::MOVING_DOWN);
        }

        void Blind::moveToPercent(int percent) {
            if (_currentState == BlindState::HOMING || _currentState == BlindState::FAULT) return;

            percent = std::max(0, std::min(100, percent));
            int32_t target = percentToSteps(percent);

            int32_t current = _motor.getCurrentPosition();
            if (target < current) {
                setState(BlindState::MOVING_UP);
            } else if (target > current) {
                setState(BlindState::MOVING_DOWN);
            } else {
                return; // Already there
            }

            _motor.moveTo(target);
        }

        void Blind::stop() {
            _motor.stop();
            setState(BlindState::IDLE); // update() will catch the actual stop
        }

        void Blind::startHoming() {
            if (!_config.hasLimitSwitches || !_bottomLimit) return; // Cannot home without bottom limit

            setState(BlindState::HOMING);
            _homingState = HomingState::MOVING_TO_BOTTOM;
            _motor.setSpeedInHz(_config.homingSpeed);

            // Move huge distance down
            _motor.move(1000000);
        }

        int Blind::getPositionPercent() {
            return stepsToPercent(_motor.getCurrentPosition());
        }

        int32_t Blind::getCurrentSteps() {
            return _motor.getCurrentPosition();
        }

        BlindState Blind::getState() {
            return _currentState;
        }

        void Blind::setState(BlindState newState) {
            _currentState = newState;
        }

        int32_t Blind::percentToSteps(int percent) {
            return (int32_t)((double)percent / 100.0 * _config.maxTravelSteps);
        }

        int Blind::stepsToPercent(int32_t steps) {
            if (_config.maxTravelSteps == 0) return 0;
            int pc = (int)((double)steps / _config.maxTravelSteps * 100.0);
            return std::max(0, std::min(100, pc));
        }

        std::string Blind::getStateString() {
            switch (_currentState) {
                case BlindState::UNKNOWN: return "UNKNOWN";
                case BlindState::IDLE: return "IDLE";
                case BlindState::MOVING_UP: return "MOVING_UP";
                case BlindState::MOVING_DOWN: return "MOVING_DOWN";
                case BlindState::HOMING: return "HOMING";
                case BlindState::FAULT: return "FAULT";
                default: return "UNKNOWN";
            }
        }
    }
}
