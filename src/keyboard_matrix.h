#pragma once

#include <vector>
#include <set>
#include "driver/gpio.h"

class KeyboardMatrix {
public:
    struct KeyPosition {
        uint8_t row;
        uint8_t col;

        bool operator<(const KeyPosition& other) const {
            return (row < other.row) || (row == other.row && col < other.col);
        }
    };

    KeyboardMatrix(const std::vector<gpio_num_t>& row_pins, 
                  const std::vector<gpio_num_t>& col_pins,
                  bool diodes_to_rows = true);
    
    ~KeyboardMatrix();

    // Initialize GPIO pins and setup matrix
    void init();

    // Scan the matrix and return set of pressed key positions
    std::set<KeyPosition> scan();

    // Get dimensions
    size_t getNumRows() const { return row_pins.size(); }
    size_t getNumCols() const { return col_pins.size(); }

private:
    std::vector<gpio_num_t> row_pins;
    std::vector<gpio_num_t> col_pins;
    bool diodes_to_rows;  // Direction of diodes: true if pointing to rows, false if to columns

    void configureInputPin(gpio_num_t pin);
    void configureOutputPin(gpio_num_t pin);
    void setPin(gpio_num_t pin, bool high);
    bool readPin(gpio_num_t pin);
};
