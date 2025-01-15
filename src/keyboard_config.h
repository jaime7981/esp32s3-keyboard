#pragma once

#include "driver/gpio.h"
#include <vector>
#include <map>
#include <string>
#include <cstdint>

// Key codes - you can expand this enum based on your needs
enum class KeyCode : uint16_t {
    NONE = 0x0000,
    
    // Basic keys
    A = 0x0004, B = 0x0005, C = 0x0006, D = 0x0007,
    E = 0x0008, F = 0x0009, G = 0x000A, H = 0x000B,
    I = 0x000C, J = 0x000D, K = 0x000E, L = 0x000F,
    M = 0x0010, N = 0x0011, O = 0x0012, P = 0x0013,
    Q = 0x0014, R = 0x0015, S = 0x0016, T = 0x0017,
    U = 0x0018, V = 0x0019, W = 0x001A, X = 0x001B,
    Y = 0x001C, Z = 0x001D,
    
    // Numbers
    NUM_1 = 0x001E, NUM_2 = 0x001F, NUM_3 = 0x0020,
    NUM_4 = 0x0021, NUM_5 = 0x0022, NUM_6 = 0x0023,
    NUM_7 = 0x0024, NUM_8 = 0x0025, NUM_9 = 0x0026,
    NUM_0 = 0x0027,
    
    // Special keys
    ENTER = 0x0028,
    ESCAPE = 0x0029,
    BACKSPACE = 0x002A,
    TAB = 0x002B,
    SPACE = 0x002C,
    
    // Modifiers
    SHIFT_LEFT = 0x00E1,
    SHIFT_RIGHT = 0x00E5,
    CTRL_LEFT = 0x00E0,
    CTRL_RIGHT = 0x00E4,
    ALT_LEFT = 0x00E2,
    ALT_RIGHT = 0x00E6,
    GUI = 0x00E3  // Windows/Command key
};

struct KeyboardConfig {
    // Physical layout configuration
    std::vector<gpio_num_t> row_pins;
    std::vector<gpio_num_t> col_pins;
    bool diodes_to_rows;
    
    // Keymap configuration
    struct KeyPosition {
        uint8_t row;
        uint8_t col;
        
        bool operator<(const KeyPosition& other) const {
            return (row < other.row) || (row == other.row && col < other.col);
        }
    };
    
    // Maps physical positions to key codes
    std::map<KeyPosition, KeyCode> keymap;
    
    // Keyboard metadata
    std::string keyboard_name;
    std::string manufacturer;
    std::string firmware_version;
    
    // Scanning configuration
    uint32_t debounce_ms;
    uint32_t scan_interval_ms;
    
    // Constructor with default values
    KeyboardConfig();
    
    // Load configuration from a file
    bool loadFromFile(const char* filename);
    
    // Save configuration to a file
    bool saveToFile(const char* filename) const;
    
    // Get key code for a position
    KeyCode getKeyCode(uint8_t row, uint8_t col) const;
};

// Global keyboard configuration
extern KeyboardConfig g_keyboard_config;
