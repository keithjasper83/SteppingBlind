#include <unity.h>
#include "mocks/MockHardware.h"
#include "domain/Blind.h"

using namespace Blinds::Domain;

MockMotor* motor;
MockLimitSwitch* topLimit;
MockLimitSwitch* bottomLimit;
MockStorage* storage;
Blind* blind;

void setUp(void) {
    motor = new MockMotor();
    topLimit = new MockLimitSwitch();
    bottomLimit = new MockLimitSwitch();
    storage = new MockStorage();

    BlindConfig config;
    config.maxTravelSteps = 1000;
    config.hasLimitSwitches = true;

    blind = new Blind(*motor, topLimit, bottomLimit, *storage, config);
}

void tearDown(void) {
    delete blind;
    delete motor;
    delete topLimit;
    delete bottomLimit;
    delete storage;
}

void test_initialization_unknown(void) {
    storage->savedPos = -1; // Invalid
    blind->initialize();
    TEST_ASSERT_EQUAL(BlindState::UNKNOWN, blind->getState());
}

void test_initialization_restores_pos(void) {
    storage->savedPos = 500;
    blind->initialize();
    TEST_ASSERT_EQUAL(BlindState::IDLE, blind->getState());
    TEST_ASSERT_EQUAL(500, motor->getCurrentPosition());
}

void test_move_up(void) {
    storage->savedPos = 500;
    blind->initialize();

    blind->moveUp();
    TEST_ASSERT_EQUAL(BlindState::MOVING_UP, blind->getState());
    TEST_ASSERT_EQUAL(0, motor->targetPos);
    TEST_ASSERT_TRUE(motor->running);
}

void test_move_down(void) {
    storage->savedPos = 500;
    blind->initialize();

    blind->moveDown();
    TEST_ASSERT_EQUAL(BlindState::MOVING_DOWN, blind->getState());
    TEST_ASSERT_EQUAL(1000, motor->targetPos);
    TEST_ASSERT_TRUE(motor->running);
}

void test_limit_switch_stops_up(void) {
    storage->savedPos = 500;
    blind->initialize();
    blind->moveUp();

    // Simulate hitting limit
    topLimit->triggered = true;
    blind->update();

    TEST_ASSERT_FALSE(motor->running);
    TEST_ASSERT_EQUAL(0, motor->getCurrentPosition());
    TEST_ASSERT_EQUAL(BlindState::IDLE, blind->getState());
}

void test_homing_sequence(void) {
    storage->savedPos = -1;
    blind->initialize();

    blind->startHoming();
    TEST_ASSERT_EQUAL(BlindState::HOMING, blind->getState());
    TEST_ASSERT_TRUE(motor->running);
    // Should be moving down (large positive number)
    TEST_ASSERT_TRUE(motor->targetPos > 1000);

    // Simulate hitting bottom limit
    bottomLimit->triggered = true;
    blind->update();

    // Should stop, set pos to max, and retract immediately
    TEST_ASSERT_TRUE(motor->running);
    TEST_ASSERT_EQUAL(1000, motor->getCurrentPosition());
    TEST_ASSERT_EQUAL(900, motor->targetPos);

    // Simulate retraction finish
    motor->completeMove();
    blind->update();

    TEST_ASSERT_EQUAL(BlindState::IDLE, blind->getState());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initialization_unknown);
    RUN_TEST(test_initialization_restores_pos);
    RUN_TEST(test_move_up);
    RUN_TEST(test_move_down);
    RUN_TEST(test_limit_switch_stops_up);
    RUN_TEST(test_homing_sequence);
    UNITY_END();
    return 0;
}
