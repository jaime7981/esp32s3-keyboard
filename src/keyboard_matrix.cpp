#include "keyboard_matrix.h"
#include "esp_log.h"
#include "rom/ets_sys.h"

static const char* TAG = "KeyboardMatrix";

KeyboardMatrix::KeyboardMatrix(const std::vector<gpio_num_t>& row_pins,
                             const std::vector<gpio_num_t>& col_pins,
                             bool diodes_to_rows)
    : row_pins(row_pins)
    , col_pins(col_pins)
    , diodes_to_rows(diodes_to_rows) {
}

KeyboardMatrix::~KeyboardMatrix() {
    // Reset all pins to input mode for safety
    for (auto pin : row_pins) {
        gpio_reset_pin(pin);
    }
    for (auto pin : col_pins) {
        gpio_reset_pin(pin);
    }
}

void KeyboardMatrix::init() {
    ESP_LOGI(TAG, "Initializing keyboard matrix with %d rows and %d columns", 
             row_pins.size(), col_pins.size());

    // Configure row pins
    for (auto pin : row_pins) {
        if (diodes_to_rows) {
            configureInputPin(pin);
        } else {
            configureOutputPin(pin);
        }
    }

    // Configure column pins
    for (auto pin : col_pins) {
        if (diodes_to_rows) {
            configureOutputPin(pin);
        } else {
            configureInputPin(pin);
        }
    }
}

void KeyboardMatrix::configureInputPin(gpio_num_t pin) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
}

void KeyboardMatrix::configureOutputPin(gpio_num_t pin) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
}

void KeyboardMatrix::setPin(gpio_num_t pin, bool high) {
    gpio_set_level(pin, high ? 1 : 0);
}

bool KeyboardMatrix::readPin(gpio_num_t pin) {
    return gpio_get_level(pin) == 1;
}

std::set<KeyboardMatrix::KeyPosition> KeyboardMatrix::scan() {
    std::set<KeyPosition> pressed_keys;
    
    const auto& strobe_pins = diodes_to_rows ? col_pins : row_pins;
    const auto& read_pins = diodes_to_rows ? row_pins : col_pins;

    // Set all strobe pins to low initially
    for (auto pin : strobe_pins) {
        setPin(pin, false);
    }

    // Scan each strobe line
    for (size_t strobe_idx = 0; strobe_idx < strobe_pins.size(); strobe_idx++) {
        // Set current strobe pin high
        setPin(strobe_pins[strobe_idx], true);
        
        // Small delay to allow signal propagation
        ets_delay_us(5);

        // Read all input pins
        for (size_t read_idx = 0; read_idx < read_pins.size(); read_idx++) {
            if (readPin(read_pins[read_idx])) {
                KeyPosition pos;
                if (diodes_to_rows) {
                    pos.row = read_idx;
                    pos.col = strobe_idx;
                } else {
                    pos.row = strobe_idx;
                    pos.col = read_idx;
                }
                pressed_keys.insert(pos);
            }
        }

        // Set current strobe pin back to low
        setPin(strobe_pins[strobe_idx], false);
    }

    return pressed_keys;
}
