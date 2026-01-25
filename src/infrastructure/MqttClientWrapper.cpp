#include "MqttClientWrapper.h"
#include <Arduino.h>

namespace Blinds {
    namespace Infrastructure {

        MqttClientWrapper::MqttClientWrapper(const char* host, uint16_t port, const char* clientId) {
            _mqttClient.setServer(host, port);
            _mqttClient.setClientId(clientId);

            using namespace std::placeholders;
            _mqttClient.onConnect(std::bind(&MqttClientWrapper::onConnect, this, _1));
            _mqttClient.onDisconnect(std::bind(&MqttClientWrapper::onDisconnect, this, _1));
            _mqttClient.onMessage(std::bind(&MqttClientWrapper::onMessage, this, _1, _2, _3, _4, _5, _6));
        }

        void MqttClientWrapper::setCredentials(const char* user, const char* pass) {
            _mqttClient.setCredentials(user, pass);
        }

        void MqttClientWrapper::begin() {
            _mqttClient.connect();
        }

        void MqttClientWrapper::publish(const std::string& topic, const std::string& payload, bool retain) {
            if (_connected) {
                _mqttClient.publish(topic.c_str(), 0, retain, payload.c_str());
            }
        }

        void MqttClientWrapper::subscribe(const std::string& topic, Interfaces::MqttCallback callback) {
            _subscriptions.push_back({topic, callback});
            if (_connected) {
                _mqttClient.subscribe(topic.c_str(), 0);
            }
        }

        bool MqttClientWrapper::isConnected() {
            return _connected;
        }

        void MqttClientWrapper::onConnect(bool sessionPresent) {
            _connected = true;
            Serial.println("MQTT Connected");
            // Resubscribe
            for (const auto& sub : _subscriptions) {
                _mqttClient.subscribe(sub.topic.c_str(), 0);
            }
        }

        void MqttClientWrapper::onDisconnect(AsyncMqttClientDisconnectReason reason) {
            _connected = false;
            Serial.println("MQTT Disconnected");
            // Reconnect logic is usually handled by a timer in main loop,
            // but AsyncMqttClient doesn't autoreconnect?
            // We should use a timer in loop() or just rely on main loop calling begin() if needed.
            // For now, we leave it to the application to manage reconnection or add a timer here.
        }

        void MqttClientWrapper::onMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
            std::string t(topic);
            std::string p(payload, len);

            // Find matching subscription
            for (const auto& sub : _subscriptions) {
                if (sub.topic == t) {
                    sub.callback(t, p);
                }
            }
        }
    }
}
