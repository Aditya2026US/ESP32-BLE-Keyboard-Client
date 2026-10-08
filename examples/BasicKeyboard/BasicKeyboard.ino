#include <Arduino.h>
#include <BleKeyboardClient.h>
#include "HidKeymap.h"

BleKeyboardClient hid;

bool shiftPressed = false;

void on_key_pressed(bool is_modifier, uint8_t key)
{
    if (is_modifier)
    {
        if (key == 0x02 || key == 0x20)   // Left Shift or Right Shift
            shiftPressed = true;

        return;
    }

    char c = hidToAscii(key, shiftPressed);

    if (c != 0)
    {
        Serial.print("Character: ");
        Serial.println(c);
    }
}

void on_key_released(bool is_modifier, uint8_t key)
{
    if (is_modifier)
    {
        if (key == 0x02 || key == 0x20)
            shiftPressed = false;
    }
}

void setup()
{
    Serial.begin(115200);
    
    // Set target keyboard name (adjust if using another keyboard)
    hid.setTargetDeviceName("ZEB-KEYPAD A1");
    
    hid.begin("ESP32 BLE HID");

    BLEKeyboard &keyboard = hid.get_keyboard();

    keyboard.on_key_pressed(on_key_pressed);
    keyboard.on_key_released(on_key_released);
}

void loop()
{
    hid.loop();
}
