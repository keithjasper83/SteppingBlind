#pragma once
#include <cstdint>
#include <string>
#include <functional>

namespace Blinds {
    namespace Interfaces {

        // Direction enum
        enum class Direction {
            STOPPED,
            UP,
            DOWN
        };

        // Motor Interface
        class IMotor {
        public:
            virtual ~IMotor() = default;
            virtual void setSpeedInHz(uint32_t speed) = 0;
            virtual void setAcceleration(uint32_t accel) = 0;
            virtual void move(int32_t steps) = 0; // Relative move
            virtual void moveTo(int32_t position) = 0; // Absolute move
            virtual void stop() = 0;
            virtual void emergencyStop() = 0;
            virtual bool isRunning() = 0;
            virtual int32_t getCurrentPosition() = 0;
            virtual void setCurrentPosition(int32_t position) = 0;
            virtual void enable(bool enabled) = 0;
        };

        // Limit Switch Interface
        class ILimitSwitch {
        public:
            virtual ~ILimitSwitch() = default;
            virtual bool isTriggered() = 0;
        };

        // Persistence Interface
        class IStorage {
        public:
            virtual ~IStorage() = default;
            virtual void savePosition(int32_t position) = 0;
            virtual int32_t loadPosition() = 0;
            virtual void saveConfig(const std::string& configJson) = 0;
            virtual std::string loadConfig() = 0;
        };
    }
}
