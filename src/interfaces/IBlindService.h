#pragma once
#include <string>

namespace Blinds {
    namespace Interfaces {
        class IBlindService {
        public:
            virtual ~IBlindService() = default;
            virtual void moveUp() = 0;
            virtual void moveDown() = 0;
            virtual void stop() = 0;
            virtual void setPosition(int percent) = 0;
            virtual void home() = 0;
            virtual std::string getStateJson() = 0;
        };
    }
}
