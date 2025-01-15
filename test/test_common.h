#pragma once

#include <unity.h>

// Common test functions
void setUp(void);
void tearDown(void);

// Keyboard Config Tests
void test_keyboard_config_default_values(void);
void test_keyboard_config_save_and_load(void);
void test_keyboard_config_keymap(void);

// Keyboard Matrix Tests
void test_keyboard_matrix_initialization(void);
void test_keyboard_matrix_scanning(void);
void test_keyboard_matrix_multiple_keys(void);

// Keyboard Controller Tests
void test_keyboard_controller_initialization(void);
void test_keyboard_controller_key_press_detection(void);
void test_keyboard_controller_key_release_detection(void);
