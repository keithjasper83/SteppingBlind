#include "BlindController.h"
#include <Arduino.h>

namespace Blinds {
    namespace Application {

        BlindController::BlindController(Domain::Blind& blind, Interfaces::IMqttClient& mqttClient)
            : _blind(blind), _mqttClient(mqttClient) {}

        void BlindController::setup() {
            _blind.initialize();

            // MQTT Subscriptions
            using namespace std::placeholders;
            // Topic for JSON commands: {"cmd": "move_up"} or {"position": 50}
            _mqttClient.subscribe("blind/set", std::bind(&BlindController::onMqttMessage, this, _1, _2));

            // Simple command topics
            _mqttClient.subscribe("blind/cmd", std::bind(&BlindController::onMqttMessage, this, _1, _2));
            _mqttClient.subscribe("blind/position/set", std::bind(&BlindController::onMqttMessage, this, _1, _2));
        }

        void BlindController::loop() {
            _blind.update();

            // Check for state changes or interval
            // For simplicity, just check if JSON changed
            std::string currentState = getStateJson();
            if (currentState != _lastStateJson || (millis() - _lastPublishTime > PUBLISH_INTERVAL && _blind.getState() != Domain::BlindState::IDLE)) {
                publishState();
                _lastStateJson = currentState;
                _lastPublishTime = millis();
            }
        }

        void BlindController::moveUp() {
            _blind.moveUp();
            publishState(true);
        }

        void BlindController::moveDown() {
            _blind.moveDown();
            publishState(true);
        }

        void BlindController::stop() {
            _blind.stop();
            publishState(true);
        }

        void BlindController::setPosition(int percent) {
            _blind.moveToPercent(percent);
            publishState(true);
        }

        void BlindController::home() {
            _blind.startHoming();
            publishState(true);
        }

        std::string BlindController::getStateJson() {
            StaticJsonDocument<256> doc;
            doc["position"] = _blind.getPositionPercent();
            doc["steps"] = _blind.getCurrentSteps();
            doc["state"] = _blind.getStateString();

            std::string output;
            serializeJson(doc, output);
            return output;
        }

        void BlindController::publishState(bool force) {
            std::string json = getStateJson();
            _mqttClient.publish("blind/state", json, true);
        }

        void BlindController::onMqttMessage(const std::string& topic, const std::string& payload) {
            if (topic == "blind/cmd") {
                if (payload == "UP") moveUp();
                else if (payload == "DOWN") moveDown();
                else if (payload == "STOP") stop();
                else if (payload == "HOME") home();
            }
            else if (topic == "blind/position/set") {
                try {
                    int pos = std::stoi(payload);
                    setPosition(pos);
                } catch (...) {}
            }
            else if (topic == "blind/set") {
                StaticJsonDocument<200> doc;
                DeserializationError error = deserializeJson(doc, payload);
                if (!error) {
                    if (doc.containsKey("cmd")) {
                        const char* cmd = doc["cmd"];
                        if (strcmp(cmd, "up") == 0) moveUp();
                        else if (strcmp(cmd, "down") == 0) moveDown();
                        else if (strcmp(cmd, "stop") == 0) stop();
                        else if (strcmp(cmd, "home") == 0) home();
                    }
                    if (doc.containsKey("position")) {
                         setPosition(doc["position"]);
                    }
                }
            }
        }
    }
}
