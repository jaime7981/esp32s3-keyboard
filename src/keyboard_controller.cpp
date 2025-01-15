#include "keyboard_controller.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char* TAG = "KeyboardController";

KeyboardController::KeyboardController(std::unique_ptr<KeyboardMatrix> matrix)
    : matrix(std::move(matrix))
    , task_handle(nullptr)
    , running(false) {
    ESP_LOGI(TAG, "KeyboardController initialized");
}

KeyboardController::~KeyboardController() {
    stop();
}

void KeyboardController::start() {
    if (!running) {
        running = true;
        BaseType_t res = xTaskCreate(
            scanTask,
            TASK_NAME,
            STACK_SIZE,
            this,
            TASK_PRIORITY,
            &task_handle
        );
        
        if (res != pdPASS) {
            ESP_LOGE(TAG, "Failed to create keyboard scan task");
            running = false;
        } else {
            ESP_LOGI(TAG, "Keyboard scan task started");
        }
    }
}

void KeyboardController::stop() {
    if (running) {
        running = false;
        if (task_handle != nullptr) {
            vTaskDelete(task_handle);
            task_handle = nullptr;
        }
        ESP_LOGI(TAG, "Keyboard scan task stopped");
    }
}

void KeyboardController::addListener(KeyEventCallback callback) {
    listeners.push_back(callback);
}

void KeyboardController::scanTask(void* arg) {
    KeyboardController* controller = static_cast<KeyboardController*>(arg);
    TickType_t last_wake_time = xTaskGetTickCount();
    
    while (controller->running) {
        // Scan the matrix
        auto new_state = controller->matrix->scan();
        
        // Process the results
        controller->processScanResults(new_state);
        
        // Wait for next scan interval
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(SCAN_INTERVAL_MS));
    }
    
    vTaskDelete(nullptr);
}

void KeyboardController::processScanResults(const std::set<KeyboardMatrix::KeyPosition>& new_state) {
    uint32_t current_time = esp_timer_get_time() / 1000; // Convert to milliseconds
    
    // Find newly pressed keys
    for (const auto& pos : new_state) {
        if (current_state.find(pos) == current_state.end()) {
            KeyEvent event{pos, KeyEvent::Type::PRESSED, current_time};
            for (const auto& listener : listeners) {
                listener(event);
            }
        }
    }
    
    // Find released keys
    for (const auto& pos : current_state) {
        if (new_state.find(pos) == new_state.end()) {
            KeyEvent event{pos, KeyEvent::Type::RELEASED, current_time};
            for (const auto& listener : listeners) {
                listener(event);
            }
        }
    }
    
    // Update current state
    current_state = new_state;
}
