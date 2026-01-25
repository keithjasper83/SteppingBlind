#pragma once
#include "../interfaces/HardwareInterfaces.h"
#include <Preferences.h>

namespace Blinds {
    namespace Infrastructure {
        class NvsStorage : public Interfaces::IStorage {
        public:
            NvsStorage();
            void savePosition(int32_t position) override;
            int32_t loadPosition() override;
            void saveConfig(const std::string& configJson) override;
            std::string loadConfig() override;
        private:
            Preferences _prefs;
        };
    }
}
