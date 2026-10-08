#include <Arduino.h>
#include <BleKeyboardClient.h>
#include "HidKeymap.h"

BleKeyboardClient hid;

bool shiftPressed = false;

void on_key_pressed(bool is_modifier, uint8_t key)
{
    if (is_modifier)
    {
        if (key == 0x02 || key == 0x20)
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
    delay(1000);

    Serial.println("\n========================================");
    Serial.println(" Bluetooth Device Selector");
    Serial.println("========================================");

    // Disable automatic background scanning and auto-connecting
    hid.setAutoConnect(false);
    hid.begin("ESP32 Device Selector");

    BLEKeyboard &keyboard = hid.get_keyboard();
    keyboard.on_key_pressed(on_key_pressed);
    keyboard.on_key_released(on_key_released);

    // 1. Scan for Bluetooth devices
    Serial.println("Scanning for Bluetooth devices (5 seconds)...");
    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->setActiveScan(true);
    NimBLEScanResults results = pScan->getResults(5000, false);

    size_t count = results.getCount();
    if (count == 0)
    {
        Serial.println("No devices found. Please reset the ESP32 to scan again.");
        return;
    }

    // 2. Clearly list them with numbers
    Serial.printf("\nFound %d device(s):\n", count);
    Serial.println("----------------------------------------");
    for (size_t i = 0; i < count; i++)
    {
        const NimBLEAdvertisedDevice* dev = results.getDevice(i);
        String name = dev->getName().c_str();
        if (name.length() == 0)
            name = "[Unnamed Device]";

        Serial.printf("%d. %s\n", i + 1, name.c_str());
    }
    Serial.println("----------------------------------------");
    Serial.printf("Enter device number (1-%d): ", count);

    // 3. Wait for user to enter a number through the Serial Monitor
    while (Serial.available() == 0)
    {
        delay(50);
    }

    int choice = Serial.parseInt();
    Serial.println(choice);

    // Flush any leftover newline or carriage return characters
    while (Serial.available())
        Serial.read();

    if (choice < 1 || choice > (int)count)
    {
        Serial.println("Invalid selection. Please reset the ESP32 to try again.");
        return;
    }

    // 4. Connect only to the device corresponding to the selected number
    const NimBLEAdvertisedDevice* selected = results.getDevice(choice - 1);
    Serial.printf("\nConnecting to: %s [%s]...\n",
                  selected->getName().c_str(),
                  selected->getAddress().toString().c_str());

    if (hid.connect(selected))
    {
        Serial.println("\n>>> Connected successfully! <<<\nReady for keyboard input:");
    }
    else
    {
        Serial.println("\n>>> Connection failed! Make sure the keyboard is in pairing mode. <<<");
    }
}

void loop()
{
    hid.loop();
}
