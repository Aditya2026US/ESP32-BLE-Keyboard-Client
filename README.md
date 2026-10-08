# ESP32-BLE-Keyboard-Client

A lightweight Arduino/PlatformIO library that turns an ESP32 into a **Bluetooth Low Energy (BLE) Keyboard Host/Central**. It allows an ESP32 to scan for, pair with, and receive keystrokes from physical Bluetooth keyboards and numerical keypads.

Powered by **NimBLE-Arduino**, this library uses significantly less RAM and flash memory compared to standard Bluedroid BLE implementations.

---

## Features

- **BLE Central / Host Mode**: Connects directly to external Bluetooth keyboards and keypads (unlike peripheral libraries that only emulate a keyboard).
- **Flexible Connection Options**: Connect by target device name, auto-discover HID keyboards, or select interactively from scan results.
- **Boot Protocol Keyboard Support**: Handles standard 8-byte HID reports with full modifier state detection (Ctrl, Alt, Shift, GUI).
- **Modifier Combination Detection**: Distinguishes shortcut combinations such as `Ctrl+A`, `Alt+A`, `Shift+A`, `Ctrl+Alt+Delete`, and `Shift+Tab`.
- **Caps Lock State Tracking**: Tracks Caps Lock toggles and automatically handles uppercase/lowercase letter casing.
- **Comprehensive Keymap**: Translates HID scan codes into ASCII characters (letters, numbers, punctuation, symbols, keypad) and identifies special keys (Enter, Space, Backspace, Tab, Escape, Arrows, Function keys F1–F12).
- **Configurable Debug Output**: Internal connection logs and raw report dumps can be enabled for troubleshooting or silenced for clean application output.

---

## Hardware Compatibility

This library is compatible with any ESP32 board that supports Bluetooth Low Energy:
- ESP32 (Original)
- ESP32-S3
- ESP32-C3
- ESP32-C6

### Supported Frameworks
- **Arduino Framework** (Arduino IDE 1.8.x, 2.x)
- **PlatformIO**

---

## Installation

### PlatformIO
Add the library to your `platformio.ini` using either the official registry name or the GitHub URL:

```ini
lib_deps =
    aditya-iot/ESP32-BLE-Keyboard-Client
    # Or directly from GitHub:
    # https://github.com/Aditya2026US/ESP32-BLE-Keyboard-Client.git
```

> **Note:** The dependency on `h2zero/NimBLE-Arduino` is declared in `library.json` and will be installed automatically by PlatformIO.

### Arduino IDE
1. **Via Library Manager (Recommended):**
   - Go to **Tools** -> **Manage Libraries...**
   - Search for `ESP32-BLE-Keyboard-Client` and click **Install**
   
   *Or via ZIP:*
   - Download this repository as a `.zip` file (**Code** -> **Download ZIP** on GitHub).
   - In Arduino IDE, go to **Sketch** -> **Include Library** -> **Add .ZIP Library...** and select the downloaded file.

2. Install **NimBLE-Arduino** (version 2.x or later):
   - Go to **Tools** -> **Manage Libraries...**
   - Search for `NimBLE-Arduino` by `h2zero`
   - Click **Install**

---

## Included Examples

This library includes three ready-to-use examples located in the `examples/` directory:

### 1. [BasicKeyboard](examples/BasicKeyboard/BasicKeyboard.ino)
* **Best for:** Standard, set-and-forget projects where you want the ESP32 to automatically find and connect to a specific keyboard.
* **How it works:** Sets the target keyboard name (e.g. `ZEB-KEYPAD A1`), scans in intervals, displays nearby Bluetooth devices on the Serial Monitor, and connects automatically when the target device is discovered. Displays connection diagnostics and raw report dumps.

### 2. [KeyboardInputTest](examples/KeyboardInputTest/KeyboardInputTest.ino)
* **Best for:** Testing keystroke reception, modifier keys, and special key mappings with clean output.
* **How it works:** Silences internal library debug logs using `hid.setDebug(false)`. Prints normal text smoothly (`abcd 1234 efgh`), formats shortcut combinations (`Ctrl+A`, `Alt+A`, `Shift+A`), tracks Caps Lock toggles (`[CapsLock: ON]` / `[CapsLock: OFF]`), and labels non-printable keys (`[Tab]`, `[Escape]`, `[Up]`, `[Down]`, `[F1]`-`[F12]`).

### 3. [DeviceSelector](examples/DeviceSelector/DeviceSelector.ino)
* **Best for:** Projects where the keyboard name is not known in advance, or when multiple Bluetooth devices are nearby.
* **How it works:** Disables background auto-connection. Scans nearby devices for 5 seconds, displays a numbered list of found devices in the Serial Monitor, and waits for user input. Type the corresponding number to connect directly to that device.

---

## API Reference

### `BleKeyboardClient` (or `BLEHIDClient`)

| Method | Description |
| :--- | :--- |
| `begin(hostName, targetDeviceName)` | Initializes BLE host and sets optional target keyboard name. |
| `setTargetDeviceName(const char* name)` | Sets the keyboard name to look for during auto-scanning. |
| `getTargetDeviceName()` | Returns the current configured target device name. |
| `setAutoConnect(bool enable)` | Enables or disables background auto-scanning and auto-connecting (`true` by default). |
| `isAutoConnect()` | Returns whether auto-connect is active. |
| `setDebug(bool enable)` | Enables or disables verbose debug logs and raw report dumps. |
| `isDebug()` | Returns current debug state. |
| `connect(const NimBLEAdvertisedDevice* dev)` | Connects directly to a specific scanned device. |
| `is_connected()` | Returns `true` if the keyboard is currently connected. |
| `get_keyboard()` | Returns a reference to the `BLEKeyboard` instance. |
| `loop()` | Handles connection maintenance, scanning intervals, and event dispatching. Call in `loop()`. |

### `BLEKeyboard`

| Method | Description |
| :--- | :--- |
| `on_key_pressed(callback)` | Registers a callback: `void on_key_pressed(bool is_modifier, uint8_t key)`. |
| `on_key_released(callback)` | Registers a callback: `void on_key_released(bool is_modifier, uint8_t key)`. |
| `get_modifiers()` | Returns the current modifier bitmask (Shift, Ctrl, Alt, GUI). |
| `is_connected()` | Returns `true` if the keyboard device is connected. |

### Keymap Utilities (`HidKeymap.h`)

| Function | Description |
| :--- | :--- |
| `hidToAscii(hid, shift, capsLock)` | Translates a HID keycode into an ASCII character, applying Shift and Caps Lock rules. Returns `0` if non-printable. |
| `hidKeyToString(hid)` | Returns a human-readable name for special keys (e.g. `"Enter"`, `"Tab"`, `"Escape"`, `"Up"`, `"F1"`). |
| `hidFormatKeyCombo(modifiers, hid, capsLock)` | Formats modifier combinations into strings like `"Ctrl+A"`, `"Alt+A"`, `"Shift+A"`, `"Ctrl+Alt+Delete"`. |

---

## Troubleshooting

1. **Keyboard Not Connecting:**
   - Ensure the keyboard is in **pairing/discovery mode** (usually indicated by a rapidly blinking LED on the keyboard).
   - If using `BasicKeyboard`, verify that `setTargetDeviceName()` matches your keyboard's broadcast name exactly.
   - If the device was previously paired with a PC or phone, unpair/forget it on that device before connecting to the ESP32.
2. **Serial Monitor Output:**
   - Make sure your Serial Monitor is set to **115200 baud**.
3. **Passkey / PIN Requests:**
   - The default passkey is set to `123456` with `BLE_HS_IO_KEYBOARD_ONLY`. Most standard BLE keyboards pair automatically.

---

## License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

Copyright (c) 2026 Aditya2026US
