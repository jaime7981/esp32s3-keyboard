#pragma once

#include "keyboard_matrix.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <functional>
#include <vector>
#include <memory>

class KeyboardController {
public:
    struct KeyEvent {
        enum class Type {
            PRESSED,
            RELEASED
        };
        
        KeyboardMatrix::KeyPosition position;
        Type type;
        uint32_t timestamp;  // Timestamp in milliseconds
    };

    using KeyEventCallback = std::function<void(const KeyEvent&)>;

    KeyboardController(std::unique_ptr<KeyboardMatrix> matrix);
    ~KeyboardController();

    // Start the scanning task
    void start();
    
    // Stop the scanning task
    void stop();

    // Add a listener for key events
    void addListener(KeyEventCallback callback);

    // Get the current state of pressed keys
    const std::set<KeyboardMatrix::KeyPosition>& getCurrentState() const {
        return current_state;
    }

private:
    static void scanTask(void* arg);
    void processScanResults(const std::set<KeyboardMatrix::KeyPosition>& new_state);
    
    std::unique_ptr<KeyboardMatrix> matrix;
    std::vector<KeyEventCallback> listeners;
    std::set<KeyboardMatrix::KeyPosition> current_state;
    TaskHandle_t task_handle;
    bool running;

    static constexpr int SCAN_INTERVAL_MS = 10;  // Scan every 10ms
    static constexpr const char* TASK_NAME = "keyboard_scan";  // Fixed: added const
    static constexpr uint32_t STACK_SIZE = 4096;
    static constexpr UBaseType_t TASK_PRIORITY = 5;
};
