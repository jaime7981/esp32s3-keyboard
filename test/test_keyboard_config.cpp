#include "test_common.h"
#include "keyboard_config.h"
#include "keyboard_config.cpp"  // Include the implementation for testing
#include "nvs_flash.h"
#include "esp_err.h"

void test_keyboard_config_default_values() {
    KeyboardConfig config;
    
    TEST_ASSERT_EQUAL(true, config.diodes_to_rows);
    TEST_ASSERT_EQUAL_STRING("DIY Keyboard", config.keyboard_name.c_str());
    TEST_ASSERT_EQUAL_STRING("Custom", config.manufacturer.c_str());
    TEST_ASSERT_EQUAL_STRING("1.0.0", config.firmware_version.c_str());
    TEST_ASSERT_EQUAL(5, config.debounce_ms);
    TEST_ASSERT_EQUAL(10, config.scan_interval_ms);
}

void test_keyboard_config_save_and_load() {
    KeyboardConfig config;
    
    // Modify some values
    config.keyboard_name = "Test Keyboard";
    config.manufacturer = "Test Manufacturer";
    config.debounce_ms = 10;
    
    // Save to NVS
    TEST_ASSERT_TRUE(config.saveToFile("test_config"));
    
    // Create a new config instance
    KeyboardConfig loaded_config;
    
    // Load from NVS
    TEST_ASSERT_TRUE(loaded_config.loadFromFile("test_config"));
    
    // Verify loaded values
    TEST_ASSERT_EQUAL_STRING("Test Keyboard", loaded_config.keyboard_name.c_str());
    TEST_ASSERT_EQUAL_STRING("Test Manufacturer", loaded_config.manufacturer.c_str());
    TEST_ASSERT_EQUAL(10, loaded_config.debounce_ms);
}

void test_keyboard_config_keymap() {
    KeyboardConfig config;
    
    // Test default keymap
    TEST_ASSERT_EQUAL(KeyCode::A, config.getKeyCode(0, 0));
    TEST_ASSERT_EQUAL(KeyCode::B, config.getKeyCode(0, 1));
    TEST_ASSERT_EQUAL(KeyCode::C, config.getKeyCode(0, 2));
    
    // Test setting and getting a custom keycode
    config.keymap[{1, 1}] = KeyCode::SPACE;
    TEST_ASSERT_EQUAL(KeyCode::SPACE, config.getKeyCode(1, 1));
}
