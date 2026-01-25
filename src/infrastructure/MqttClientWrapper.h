#pragma once
#include "../interfaces/NetworkInterfaces.h"
#include <AsyncMqttClient.h>
#include <map>
#include <vector>

namespace Blinds {
    namespace Infrastructure {
        class MqttClientWrapper : public Interfaces::IMqttClient {
        public:
            MqttClientWrapper(const char* host, uint16_t port, const char* clientId);
            void setCredentials(const char* user, const char* pass);
            void begin();

            void publish(const std::string& topic, const std::string& payload, bool retain = false) override;
            void subscribe(const std::string& topic, Interfaces::MqttCallback callback) override;
            bool isConnected() override;

            // Callback handlers for AsyncMqttClient
            void onConnect(bool sessionPresent);
            void onDisconnect(AsyncMqttClientDisconnectReason reason);
            void onMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);

        private:
            AsyncMqttClient _mqttClient;
            struct Subscription {
                std::string topic;
                Interfaces::MqttCallback callback;
            };
            std::vector<Subscription> _subscriptions;
            bool _connected = false;
        };
    }
}
