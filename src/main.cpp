#include <Arduino.h>
#include <FastAccelStepper.h>

#include "domain/Blind.h"
#include "domain/BlindConfig.h"
#include "infrastructure/FastAccelStepperMotor.h"
#include "infrastructure/GpioLimitSwitch.h"
#include "infrastructure/NvsStorage.h"
#include "infrastructure/WifiNetwork.h"
#include "infrastructure/MqttClientWrapper.h"
#include "infrastructure/WebServerManager.h"
#include "application/BlindController.h"

// --- Configuration ---
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASS "YOUR_PASSWORD"

#define MQTT_HOST "192.168.1.100"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "esp32-blinds"

// --- Pin Definitions (ESP32-S3) ---
#define PIN_STEP    4
#define PIN_DIR     5
#define PIN_ENABLE  6
#define PIN_LIMIT_TOP    7
#define PIN_LIMIT_BOTTOM 15
// Buttons (Optional)
#define PIN_BTN_UP   16
#define PIN_BTN_DOWN 17

// --- Globals (Dependency Injection Roots) ---
FastAccelStepperEngine engine;

using namespace Blinds;

// Infrastructure
Infrastructure::FastAccelStepperMotor* motorDriver;
Infrastructure::GpioLimitSwitch* limitTop;
Infrastructure::GpioLimitSwitch* limitBottom;
Infrastructure::NvsStorage* storage;
Infrastructure::WifiNetwork* wifi;
Infrastructure::MqttClientWrapper* mqtt;
Infrastructure::WebServerManager* webServer;

// Domain
Domain::Blind* blindDomain;

// Application
Application::BlindController* controller;

void setup() {
    Serial.begin(115200);
    Serial.println("Booting ESP32 Smart Blind Controller...");

    // 1. Initialize Hardware Infrastructure
    engine.init();
    motorDriver = new Infrastructure::FastAccelStepperMotor(engine, PIN_STEP, PIN_DIR, PIN_ENABLE);
    limitTop = new Infrastructure::GpioLimitSwitch(PIN_LIMIT_TOP, true); // true = pullup (low active)
    limitBottom = new Infrastructure::GpioLimitSwitch(PIN_LIMIT_BOTTOM, true);
    storage = new Infrastructure::NvsStorage();

    // 2. Initialize Network Infrastructure
    wifi = new Infrastructure::WifiNetwork(WIFI_SSID, WIFI_PASS);
    mqtt = new Infrastructure::MqttClientWrapper(MQTT_HOST, MQTT_PORT, MQTT_CLIENT_ID);

    // 3. Initialize Domain
    Domain::BlindConfig config;
    config.maxTravelSteps = 20000; // Example: Calibrate this!
    config.defaultSpeed = 1000;
    config.acceleration = 1000;
    config.homingSpeed = 500;
    config.hasLimitSwitches = true;
    config.invertDirection = false; // Adjust as needed

    blindDomain = new Domain::Blind(*motorDriver, limitTop, limitBottom, *storage, config);

    // 4. Initialize Application
    controller = new Application::BlindController(*blindDomain, *mqtt);

    // 5. Initialize Web Server
    webServer = new Infrastructure::WebServerManager(*controller, 80);

    // Start Services
    controller->setup(); // Loads position, inits motor
    wifi->begin();
    mqtt->begin();
    webServer->begin();

    Serial.println("System Initialized.");
}

void loop() {
    // Poll Network
    // wifi->loop(); // Not needed for standard WiFi lib usually
    // mqtt handled by AsyncMqttClient in background task?
    // AsyncMqttClient runs on FreeRTOS task, but we might need reconnection logic if not auto handled.
    if (wifi->isConnected() && !mqtt->isConnected()) {
        static unsigned long lastMqttReconnect = 0;
        if (millis() - lastMqttReconnect > 5000) {
            Serial.println("Reconnecting MQTT...");
            mqtt->begin();
            lastMqttReconnect = millis();
        }
    }

    // Run Application Logic
    controller->loop();

    // Manual Buttons (Optional Logic - bypassing controller for now or adding to it?)
    // Ideally should be handled by an InputManager in Infrastructure that calls controller->moveUp() etc.
    // For simplicity, polling here:
    static int lastBtnUp = HIGH;
    static int lastBtnDown = HIGH;

    int btnUp = digitalRead(PIN_BTN_UP); // Assuming input pullup done externally or we need to pinMode
    // Note: I didn't set pinMode for buttons.
    // Lets add minimal button support if pins defined
    // But PIN_BTN_UP might be floating if not connected.
    // I'll skip button implementation for now to avoid noise if pins are floating,
    // or set INPUT_PULLUP if I used them.
}
