# ESP32 DIY Keyboard

A customizable keyboard firmware for ESP32 that supports matrix scanning, key mapping, and configuration persistence.

## Features

- Matrix keyboard scanning with configurable pins
- Customizable key mapping
- Configuration persistence using NVS (Non-Volatile Storage)
- Debounce handling
- Event-based key handling system
- FreeRTOS task-based scanning

## Hardware Requirements

- ESP32 development board
- Key switches arranged in a matrix configuration
- Diodes (if using diode protection)

## Project Structure

```
ESP32-DIY-KEYBOARD/
├── src/
│   ├── main.cpp              # Main application entry point
│   ├── keyboard_matrix.h/cpp # Matrix scanning implementation
│   ├── keyboard_config.h/cpp # Keyboard configuration management
│   └── keyboard_controller.h/cpp # Main keyboard controller
├── test/                     # Unit tests
└── README.md                 # This file
```

## Configuration

The keyboard can be configured through the `KeyboardConfig` class. Main configuration options include:

- Row and column pin assignments
- Diode direction (diodes to rows or columns)
- Keyboard name and manufacturer
- Debounce time
- Scan interval
- Key mapping

## Building and Flashing

This project uses PlatformIO for building and flashing. To build and flash:

1. Install PlatformIO
2. Clone this repository
3. Connect your ESP32
4. Run:
   ```bash
   pio run -t upload
   ```

## Testing

Run the tests using:
```bash
pio test
```

## Contributing

1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.
