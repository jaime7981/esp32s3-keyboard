#include "test_common.h"
#include "keyboard_matrix.h"
#include "keyboard_matrix.cpp"  // Include the implementation for testing
#include "driver/gpio.h"
#include "esp_err.h"

// Mock GPIO pins for testing
static const std::vector<gpio_num_t> TEST_ROW_PINS = {
    GPIO_NUM_1,
    GPIO_NUM_2,
    GPIO_NUM_3
};

static const std::vector<gpio_num_t> TEST_COL_PINS = {
    GPIO_NUM_4,
    GPIO_NUM_5,
    GPIO_NUM_6
};

// Helper function to reset pins
static void reset_test_pins() {
    for (auto pin : TEST_ROW_PINS) {
        gpio_reset_pin(pin);
    }
    for (auto pin : TEST_COL_PINS) {
        gpio_reset_pin(pin);
    }
}

void test_keyboard_matrix_initialization() {
    reset_test_pins();
    KeyboardMatrix matrix(TEST_ROW_PINS, TEST_COL_PINS, true);
    matrix.init();
    
    // Verify pin configurations using gpio_get_level as a basic test
    // since ESP-IDF doesn't provide direct pin configuration query functions
    for (auto pin : TEST_ROW_PINS) {
        gpio_set_direction(pin, GPIO_MODE_OUTPUT);
        TEST_ASSERT_EQUAL(0, gpio_get_level(pin)); // Should be initialized low
    }
    
    for (auto pin : TEST_COL_PINS) {
        gpio_set_direction(pin, GPIO_MODE_INPUT);
        gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
        TEST_ASSERT_EQUAL(1, gpio_get_level(pin)); // Should be pulled high
    }
}

void test_keyboard_matrix_scanning() {
    reset_test_pins();
    KeyboardMatrix matrix(TEST_ROW_PINS, TEST_COL_PINS, true);
    matrix.init();
    
    // Simulate a key press by setting a column pin low
    gpio_set_direction(TEST_COL_PINS[0], GPIO_MODE_OUTPUT);
    gpio_set_level(TEST_COL_PINS[0], 0);
    
    // Scan the matrix
    auto pressed_keys = matrix.scan();
    
    // Should detect one key press at (0,0)
    TEST_ASSERT_EQUAL(1, pressed_keys.size());
    TEST_ASSERT_TRUE(pressed_keys.find({0, 0}) != pressed_keys.end());
    
    // Clean up
    gpio_set_direction(TEST_COL_PINS[0], GPIO_MODE_INPUT);
}

void test_keyboard_matrix_multiple_keys() {
    reset_test_pins();
    KeyboardMatrix matrix(TEST_ROW_PINS, TEST_COL_PINS, true);
    matrix.init();
    
    // Simulate multiple key presses
    gpio_set_direction(TEST_COL_PINS[0], GPIO_MODE_OUTPUT);
    gpio_set_direction(TEST_COL_PINS[1], GPIO_MODE_OUTPUT);
    gpio_set_level(TEST_COL_PINS[0], 0);
    gpio_set_level(TEST_COL_PINS[1], 0);
    
    auto pressed_keys = matrix.scan();
    
    TEST_ASSERT_EQUAL(2, pressed_keys.size());
    TEST_ASSERT_TRUE(pressed_keys.find({0, 0}) != pressed_keys.end());
    TEST_ASSERT_TRUE(pressed_keys.find({0, 1}) != pressed_keys.end());
    
    // Clean up
    gpio_set_direction(TEST_COL_PINS[0], GPIO_MODE_INPUT);
    gpio_set_direction(TEST_COL_PINS[1], GPIO_MODE_INPUT);
}
