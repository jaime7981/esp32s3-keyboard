#include "test_common.h"
#include "keyboard_controller.h"
#include "keyboard_controller.cpp"  // Include the implementation for testing
#include "driver/gpio.h"
#include <memory>

// Test variables
static bool key_event_received = false;
static KeyboardController::KeyEvent last_event;

// Mock matrix for testing
class MockKeyboardMatrix : public KeyboardMatrix {
public:
    MockKeyboardMatrix() : KeyboardMatrix({GPIO_NUM_1}, {GPIO_NUM_2}, true) {}
    
    void setMockState(const std::set<KeyPosition>& state) {
        mock_state = state;
    }
    
    std::set<KeyPosition> scan() {
        return mock_state;
    }

private:
    std::set<KeyPosition> mock_state;
};

// Test callback for key events
void test_key_callback(const KeyboardController::KeyEvent& event) {
    key_event_received = true;
    last_event = event;
}

void test_keyboard_controller_initialization() {
    key_event_received = false;
    auto matrix = std::make_unique<MockKeyboardMatrix>();
    KeyboardController controller(std::move(matrix));
    
    // Add listener
    controller.addListener(test_key_callback);
    
    // Verify controller is properly initialized
    TEST_ASSERT_TRUE(controller.getCurrentState().empty());
}

void test_keyboard_controller_key_press_detection() {
    key_event_received = false;
    auto matrix = std::make_unique<MockKeyboardMatrix>();
    auto* matrix_ptr = matrix.get();
    KeyboardController controller(std::move(matrix));
    
    controller.addListener(test_key_callback);
    
    // Simulate key press
    std::set<KeyboardMatrix::KeyPosition> pressed_keys;
    pressed_keys.insert({0, 0});
    matrix_ptr->setMockState(pressed_keys);
    
    // Start controller and let it run for a brief period
    controller.start();
    vTaskDelay(pdMS_TO_TICKS(50));
    controller.stop();
    
    TEST_ASSERT_TRUE(key_event_received);
    TEST_ASSERT_EQUAL(KeyboardController::KeyEvent::Type::PRESSED, last_event.type);
    TEST_ASSERT_EQUAL(0, last_event.position.row);
    TEST_ASSERT_EQUAL(0, last_event.position.col);
}

void test_keyboard_controller_key_release_detection() {
    key_event_received = false;
    auto matrix = std::make_unique<MockKeyboardMatrix>();
    auto* matrix_ptr = matrix.get();
    KeyboardController controller(std::move(matrix));
    
    controller.addListener(test_key_callback);
    
    // Simulate key press then release
    std::set<KeyboardMatrix::KeyPosition> pressed_keys;
    pressed_keys.insert({0, 0});
    matrix_ptr->setMockState(pressed_keys);
    
    controller.start();
    vTaskDelay(pdMS_TO_TICKS(50));
    
    // Simulate key release
    pressed_keys.clear();
    matrix_ptr->setMockState(pressed_keys);
    
    vTaskDelay(pdMS_TO_TICKS(50));
    controller.stop();
    
    TEST_ASSERT_TRUE(key_event_received);
    TEST_ASSERT_EQUAL(KeyboardController::KeyEvent::Type::RELEASED, last_event.type);
}
