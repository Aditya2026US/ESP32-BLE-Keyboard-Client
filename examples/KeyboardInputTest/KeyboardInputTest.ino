#include <Arduino.h>
#include <BleKeyboardClient.h>
#include "HidKeymap.h"

BleKeyboardClient hid;

// Keyboard state tracking
bool capsLockActive = false;
uint8_t currentModifiers = 0;
bool wasConnected = false;

void on_key_pressed(bool is_modifier, uint8_t key)
{
    if (is_modifier)
    {
        currentModifiers |= key;
        return;
    }

    // 1. Caps Lock Handling
    if (key == HID_KEY_CAPSLOCK)
    {
        capsLockActive = !capsLockActive;
        Serial.println();
        Serial.println(capsLockActive ? "[CapsLock: ON]" : "[CapsLock: OFF]");
        return;
    }

    // 2. Modifier Combinations (e.g. Ctrl+A, Alt+A, Shift+A, Ctrl+Alt+Delete)
    String combo = hidFormatKeyCombo(currentModifiers, key, capsLockActive);
    if (combo.length() > 0)
    {
        Serial.println();
        Serial.println(combo);
        return;
    }

    // 3. Special Non-Printable Keys
    const char* specialName = hidKeyToString(key);
    if (specialName)
    {
        if (key == HID_KEY_SPACE)
        {
            Serial.print(' ');
        }
        else if (key == HID_KEY_ENTER)
        {
            Serial.println();
        }
        else if (key == HID_KEY_TAB)
        {
            Serial.print("[Tab]");
        }
        else if (key == HID_KEY_BACKSPACE)
        {
            Serial.print("[Backspace]");
        }
        else
        {
            Serial.printf("[%s]", specialName);
        }
        return;
    }

    // 4. Normal Printable Characters (letters, digits, symbols)
    bool isShift = (currentModifiers & 0x22) != 0;
    char c = hidToAscii(key, isShift, capsLockActive);
    if (c != 0)
    {
        Serial.print(c);
    }
}

void on_key_released(bool is_modifier, uint8_t key)
{
    if (is_modifier)
    {
        currentModifiers &= ~key;
    }
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n========================================");
    Serial.println(" ESP32 BLE Keyboard Input Test");
    Serial.println("========================================");

    // Disable raw debug reports for a clean serial output
    hid.setDebug(false);

    // Set target keyboard name (adjust if using another keyboard)
    hid.setTargetDeviceName("ZEB-KEYPAD A1");

    Serial.println("Scanning for keyboard...");
    hid.begin("ESP32 Keyboard Host");

    BLEKeyboard &keyboard = hid.get_keyboard();
    keyboard.on_key_pressed(on_key_pressed);
    keyboard.on_key_released(on_key_released);
}

void loop()
{
    hid.loop();

    // Notify user on connection transition
    if (hid.is_connected() && !wasConnected)
    {
        wasConnected = true;
        Serial.println("\n>>> Keyboard Connected! Ready for input. <<<\n");
    }
    else if (!hid.is_connected() && wasConnected)
    {
        wasConnected = false;
        Serial.println("\n>>> Keyboard Disconnected. <<<\n");
    }
}
