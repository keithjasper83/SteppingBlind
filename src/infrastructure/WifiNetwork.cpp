#include "WifiNetwork.h"
#include <Arduino.h>

namespace Blinds {
    namespace Infrastructure {

        WifiNetwork::WifiNetwork(const char* ssid, const char* password)
            : _ssid(ssid), _password(password) {}

        void WifiNetwork::begin() {
            WiFi.mode(WIFI_STA);
            WiFi.begin(_ssid, _password);
            Serial.print("Connecting to WiFi");
            // Non-blocking wait handled in loop or just let it connect in background
        }

        void WifiNetwork::loop() {
            // Reconnect logic could go here
        }

        bool WifiNetwork::isConnected() {
            return WiFi.status() == WL_CONNECTED;
        }

        std::string WifiNetwork::getIpAddress() {
            return std::string(WiFi.localIP().toString().c_str());
        }
    }
}
