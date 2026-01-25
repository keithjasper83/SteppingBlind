#pragma once
#include <string>
#include <functional>

namespace Blinds {
    namespace Interfaces {

        using MqttCallback = std::function<void(const std::string& topic, const std::string& payload)>;

        class INetwork {
        public:
            virtual ~INetwork() = default;
            virtual bool isConnected() = 0;
            virtual std::string getIpAddress() = 0;
        };

        class IMqttClient {
        public:
            virtual ~IMqttClient() = default;
            virtual void publish(const std::string& topic, const std::string& payload, bool retain = false) = 0;
            virtual void subscribe(const std::string& topic, MqttCallback callback) = 0;
            virtual bool isConnected() = 0;
        };
    }
}
