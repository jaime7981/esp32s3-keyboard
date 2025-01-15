#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "keyboard_controller.h"
#include "keyboard_config.h"
#include <esp_err.h>
#include "nvs_flash.h"

static const char* TAG = "Main";

// Example key event handler
void handleKeyEvent(const KeyboardController::KeyEvent& event) {
    const char* event_type = (event.type == KeyboardController::KeyEvent::Type::PRESSED) 
        ? "PRESSED" : "RELEASED";
    
    KeyCode keycode = g_keyboard_config.getKeyCode(event.position.row, event.position.col);
    
    ESP_LOGI(TAG, "Key %s at row %d, col %d, keycode: %d (timestamp: %lu ms)",
        event_type,
        event.position.row,
        event.position.col,
        static_cast<int>(keycode),
        event.timestamp);
}

extern "C" void app_main(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Try to load configuration from NVS
    if (!g_keyboard_config.loadFromFile("keyboard")) {
        ESP_LOGW(TAG, "Failed to load keyboard config, using defaults");
    }

    // Create and initialize the keyboard matrix
    auto matrix = std::make_unique<KeyboardMatrix>(
        g_keyboard_config.row_pins,
        g_keyboard_config.col_pins,
        g_keyboard_config.diodes_to_rows
    );
    matrix->init();

    // Create the keyboard controller
    KeyboardController controller(std::move(matrix));
    
    // Add our key event handler
    controller.addListener(handleKeyEvent);
    
    // Start the controller
    controller.start();

    ESP_LOGI(TAG, "Keyboard initialized - %s by %s (Firmware v%s)",
        g_keyboard_config.keyboard_name.c_str(),
        g_keyboard_config.manufacturer.c_str(),
        g_keyboard_config.firmware_version.c_str());

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
