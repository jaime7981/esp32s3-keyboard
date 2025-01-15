#include "test_common.h"
#include "nvs_flash.h"
#include "esp_err.h"

void setUp(void) {
    // Initialize NVS for testing
    ESP_ERROR_CHECK(nvs_flash_init());
}

void tearDown(void) {
    // Clean up NVS after testing
    ESP_ERROR_CHECK(nvs_flash_erase());
    ESP_ERROR_CHECK(nvs_flash_init());
}

void RUN_UNITY_TESTS() {
    UNITY_BEGIN();
    
    // Keyboard Config Tests
    RUN_TEST(test_keyboard_config_default_values);
    RUN_TEST(test_keyboard_config_save_and_load);
    RUN_TEST(test_keyboard_config_keymap);
    
    // Keyboard Matrix Tests
    RUN_TEST(test_keyboard_matrix_initialization);
    RUN_TEST(test_keyboard_matrix_scanning);
    RUN_TEST(test_keyboard_matrix_multiple_keys);
    
    // Keyboard Controller Tests
    RUN_TEST(test_keyboard_controller_initialization);
    RUN_TEST(test_keyboard_controller_key_press_detection);
    RUN_TEST(test_keyboard_controller_key_release_detection);
    
    UNITY_END();
}

extern "C" void app_main() {
    setUp();
    RUN_UNITY_TESTS();
    tearDown();
}
