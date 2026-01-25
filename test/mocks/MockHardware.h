#pragma once
#include "../../src/interfaces/HardwareInterfaces.h"

using namespace Blinds::Interfaces;

class MockMotor : public IMotor {
public:
    int32_t currentPos = 0;
    int32_t targetPos = 0;
    bool running = false;
    bool enabled = false;

    void setSpeedInHz(uint32_t speed) override {}
    void setAcceleration(uint32_t accel) override {}

    void move(int32_t steps) override {
        targetPos = currentPos + steps;
        running = true;
    }

    void moveTo(int32_t position) override {
        targetPos = position;
        running = true;
    }

    void stop() override {
        running = false;
        // In reality, stop might decel, but for mock instant stop
    }

    void emergencyStop() override {
        running = false;
    }

    bool isRunning() override {
        return running;
    }

    int32_t getCurrentPosition() override {
        return currentPos;
    }

    void setCurrentPosition(int32_t position) override {
        currentPos = position;
    }

    void enable(bool en) override {
        enabled = en;
    }

    // Helper to simulate completion
    void completeMove() {
        currentPos = targetPos;
        running = false;
    }
};

class MockLimitSwitch : public ILimitSwitch {
public:
    bool triggered = false;
    bool isTriggered() override {
        return triggered;
    }
};

class MockStorage : public IStorage {
public:
    int32_t savedPos = -1;
    void savePosition(int32_t position) override {
        savedPos = position;
    }
    int32_t loadPosition() override {
        return savedPos;
    }
    void saveConfig(const std::string& configJson) override {}
    std::string loadConfig() override { return ""; }
};
