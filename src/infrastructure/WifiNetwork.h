#pragma once
#include "../interfaces/NetworkInterfaces.h"
#include <WiFi.h>

namespace Blinds {
    namespace Infrastructure {
        class WifiNetwork : public Interfaces::INetwork {
        public:
            WifiNetwork(const char* ssid, const char* password);
            void begin();
            void loop(); // Maintenance if needed
            bool isConnected() override;
            std::string getIpAddress() override;

        private:
            const char* _ssid;
            const char* _password;
        };
    }
}
