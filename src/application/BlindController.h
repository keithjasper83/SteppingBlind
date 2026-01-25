#pragma once

#include "../interfaces/IBlindService.h"
#include "../domain/Blind.h"
#include "../interfaces/NetworkInterfaces.h"
#include <ArduinoJson.h>

namespace Blinds {
    namespace Application {

        class BlindController : public Interfaces::IBlindService {
        public:
            BlindController(Domain::Blind& blind, Interfaces::IMqttClient& mqttClient);

            void setup();
            void loop();

            // IBlindService implementation
            void moveUp() override;
            void moveDown() override;
            void stop() override;
            void setPosition(int percent) override;
            void home() override;
            std::string getStateJson() override;

        private:
            Domain::Blind& _blind;
            Interfaces::IMqttClient& _mqttClient;

            std::string _lastStateJson;
            unsigned long _lastPublishTime = 0;
            const unsigned long PUBLISH_INTERVAL = 1000; // Force update every 1s if moving?

            void publishState(bool force = false);
            void onMqttMessage(const std::string& topic, const std::string& payload);
        };
    }
}
