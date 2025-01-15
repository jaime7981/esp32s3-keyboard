#include "keyboard_config.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <cstring>

static const char* TAG = "KeyboardConfig";

// Global configuration instance
KeyboardConfig g_keyboard_config;

KeyboardConfig::KeyboardConfig()
    : diodes_to_rows(true)
    , keyboard_name("DIY Keyboard")
    , manufacturer("Custom")
    , firmware_version("1.0.0")
    , debounce_ms(5)
    , scan_interval_ms(10) {
    
    // Default pin configuration - modify these for your keyboard
    row_pins = {
        GPIO_NUM_1,  // Row 1
        GPIO_NUM_2,  // Row 2
        GPIO_NUM_3   // Row 3
    };
    
    col_pins = {
        GPIO_NUM_4,  // Column 1
        GPIO_NUM_5,  // Column 2
        GPIO_NUM_6   // Column 3
    };
    
    // Default keymap - modify this for your layout
    // Example: A simple 3x3 grid with letters A-I
    keymap[{0, 0}] = KeyCode::A;
    keymap[{0, 1}] = KeyCode::B;
    keymap[{0, 2}] = KeyCode::C;
    keymap[{1, 0}] = KeyCode::D;
    keymap[{1, 1}] = KeyCode::E;
    keymap[{1, 2}] = KeyCode::F;
    keymap[{2, 0}] = KeyCode::G;
    keymap[{2, 1}] = KeyCode::H;
    keymap[{2, 2}] = KeyCode::I;
}

bool KeyboardConfig::loadFromFile(const char* filename) {
    nvs_handle_t nvs_handle;
    esp_err_t err;

    // Initialize NVS
    err = nvs_open(filename, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle: %s", esp_err_to_name(err));
        return false;
    }

    // Load configuration
    size_t required_size;
    
    // Load keyboard name
    if (nvs_get_str(nvs_handle, "kb_name", nullptr, &required_size) == ESP_OK) {
        char* temp = new char[required_size];
        nvs_get_str(nvs_handle, "kb_name", temp, &required_size);
        keyboard_name = temp;
        delete[] temp;
    }

    // Load pin configurations
    uint8_t pin_count;
    if (nvs_get_u8(nvs_handle, "row_count", &pin_count) == ESP_OK) {
        row_pins.clear();
        for (int i = 0; i < pin_count; i++) {
            int32_t pin;
            char key[16];
            snprintf(key, sizeof(key), "row_pin_%d", i);
            if (nvs_get_i32(nvs_handle, key, &pin) == ESP_OK) {
                row_pins.push_back(static_cast<gpio_num_t>(pin));
            }
        }
    }

    if (nvs_get_u8(nvs_handle, "col_count", &pin_count) == ESP_OK) {
        col_pins.clear();
        for (int i = 0; i < pin_count; i++) {
            int32_t pin;
            char key[16];
            snprintf(key, sizeof(key), "col_pin_%d", i);
            if (nvs_get_i32(nvs_handle, key, &pin) == ESP_OK) {
                col_pins.push_back(static_cast<gpio_num_t>(pin));
            }
        }
    }

    // Load keymap
    uint16_t keymap_size;
    if (nvs_get_u16(nvs_handle, "keymap_size", &keymap_size) == ESP_OK) {
        keymap.clear();
        for (int i = 0; i < keymap_size; i++) {
            char key[32];
            uint8_t row, col;
            uint16_t keycode;
            
            snprintf(key, sizeof(key), "key_%d_pos", i);
            if (nvs_get_u8(nvs_handle, key, &row) != ESP_OK) continue;
            
            snprintf(key, sizeof(key), "key_%d_col", i);
            if (nvs_get_u8(nvs_handle, key, &col) != ESP_OK) continue;
            
            snprintf(key, sizeof(key), "key_%d_code", i);
            if (nvs_get_u16(nvs_handle, key, &keycode) != ESP_OK) continue;
            
            keymap[{row, col}] = static_cast<KeyCode>(keycode);
        }
    }

    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "Configuration loaded successfully");
    return true;
}

bool KeyboardConfig::saveToFile(const char* filename) const {
    nvs_handle_t nvs_handle;
    esp_err_t err;

    // Initialize NVS
    err = nvs_open(filename, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle: %s", esp_err_to_name(err));
        return false;
    }

    // Save configuration
    nvs_set_str(nvs_handle, "kb_name", keyboard_name.c_str());
    
    // Save pin configurations
    nvs_set_u8(nvs_handle, "row_count", row_pins.size());
    for (size_t i = 0; i < row_pins.size(); i++) {
        char key[16];
        snprintf(key, sizeof(key), "row_pin_%d", i);
        nvs_set_i32(nvs_handle, key, static_cast<int32_t>(row_pins[i]));
    }

    nvs_set_u8(nvs_handle, "col_count", col_pins.size());
    for (size_t i = 0; i < col_pins.size(); i++) {
        char key[16];
        snprintf(key, sizeof(key), "col_pin_%d", i);
        nvs_set_i32(nvs_handle, key, static_cast<int32_t>(col_pins[i]));
    }

    // Save keymap
    nvs_set_u16(nvs_handle, "keymap_size", keymap.size());
    int i = 0;
    for (const auto& mapping : keymap) {
        char key[32];
        snprintf(key, sizeof(key), "key_%d_pos", i);
        nvs_set_u8(nvs_handle, key, mapping.first.row);
        
        snprintf(key, sizeof(key), "key_%d_col", i);
        nvs_set_u8(nvs_handle, key, mapping.first.col);
        
        snprintf(key, sizeof(key), "key_%d_code", i);
        nvs_set_u16(nvs_handle, key, static_cast<uint16_t>(mapping.second));
        
        i++;
    }

    nvs_commit(nvs_handle);
    nvs_close(nvs_handle);
    
    ESP_LOGI(TAG, "Configuration saved successfully");
    return true;
}

KeyCode KeyboardConfig::getKeyCode(uint8_t row, uint8_t col) const {
    KeyPosition pos{row, col};
    auto it = keymap.find(pos);
    return it != keymap.end() ? it->second : KeyCode::NONE;
}
