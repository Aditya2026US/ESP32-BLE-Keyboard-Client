#pragma once

#include <Arduino.h>

// Modifier Bitmasks
#define HID_MOD_LCTRL  0x01
#define HID_MOD_LSHIFT 0x02
#define HID_MOD_LALT   0x04
#define HID_MOD_LGUI   0x08
#define HID_MOD_RCTRL  0x10
#define HID_MOD_RSHIFT 0x20
#define HID_MOD_RALT   0x40
#define HID_MOD_RGUI   0x80

// Special HID Usage IDs
#define HID_KEY_ENTER      0x28
#define HID_KEY_ESCAPE     0x29
#define HID_KEY_BACKSPACE  0x2A
#define HID_KEY_TAB        0x2B
#define HID_KEY_SPACE      0x2C
#define HID_KEY_CAPSLOCK   0x39
#define HID_KEY_F1         0x3A
#define HID_KEY_F12        0x45
#define HID_KEY_RIGHT      0x4F
#define HID_KEY_LEFT       0x50
#define HID_KEY_DOWN       0x51
#define HID_KEY_UP         0x52

/**
 * Translates standard USB HID scan code to ASCII character.
 * Returns 0 if the key is non-printable or special.
 */
char hidToAscii(uint8_t hid, bool shift = false, bool capsLock = false);

/**
 * Returns human-readable name for special non-printable keys
 * (e.g. "Enter", "Backspace", "Tab", "Escape", "F1", "ArrowUp", etc.)
 * Returns nullptr if the key is a normal printable character.
 */
const char* hidKeyToString(uint8_t hid);

/**
 * Formats key press with modifiers into readable format:
 * - "Ctrl+A"
 * - "Alt+A"
 * - "Shift+A"
 * - "Ctrl+Alt+Delete"
 * Returns empty string if it's a standard text typing keystroke.
 */
String hidFormatKeyCombo(uint8_t modifiers, uint8_t hidKey, bool capsLock = false);
