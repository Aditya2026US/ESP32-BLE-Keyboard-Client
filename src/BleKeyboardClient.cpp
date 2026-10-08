#include <Arduino.h>
#include "BleKeyboardClient.h"

bool g_ble_hid_debug_enabled = true;

void BLEHIDClient::begin(const char *device_name, bool keyboard_enabled, bool mouse_enabled) {
    NimBLEDevice::init(device_name);
    NimBLEDevice::setSecurityAuth(true, true, true);
    NimBLEDevice::setSecurityPasskey(123456);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_KEYBOARD_ONLY);
    this->keyboard_enabled = keyboard_enabled;
    pScan = NimBLEDevice::getScan();
    pScan->setScanCallbacks(&scan_callbacks, false);
    pScan->setInterval(100);
    pScan->setWindow(100);
    pScan->setActiveScan(true);
    if (this->keyboard_enabled && this->auto_connect)
        start_scan();
}

void BLEHIDClient::begin(const char *device_name, const char *target_keyboard_name) {
    if (target_keyboard_name) {
        this->target_device_name = target_keyboard_name;
    }
    begin(device_name, true, false);
}

void BLEHIDClient::setAutoConnect(bool enable) {
    this->auto_connect = enable;
    if (!enable && pScan && pScan->isScanning()) {
        pScan->stop();
    }
}

bool BLEHIDClient::isAutoConnect() const {
    return auto_connect;
}

void BLEHIDClient::setTargetDeviceName(const char* name) {
    if (name) {
        this->target_device_name = name;
    } else {
        this->target_device_name = "";
    }
}

const char* BLEHIDClient::getTargetDeviceName() const {
    return target_device_name.c_str();
}

bool BLEHIDClient::connect(const NimBLEAdvertisedDevice* device) {
    if (!device) return false;
    return keyboard.connect(device);
}

bool BLEHIDClient::is_connected() {
    return keyboard.is_connected();
}

void BLEHIDClient::setDebug(bool enable) {
    g_ble_hid_debug_enabled = enable;
}

bool BLEHIDClient::isDebug() const {
    return g_ble_hid_debug_enabled;
}

void BLEHIDClient::loop() {
    if (!keyboard_enabled)
        return;

    // In manual mode (e.g. DeviceSelector), loop does not auto-scan or auto-connect
    if (!auto_connect)
        return;

    const NimBLEScanResults *results = scan_callbacks.results;
    scan_callbacks.results = nullptr;

    if (results && results->getCount() > 0) {

        BLE_DEBUG_PRINTF("Found %d device(s)\n", results->getCount());

        for (auto device : *results) {

            BLE_DEBUG_PRINTLN("--------------------------------");
            BLE_DEBUG_PRINTF("Name    : %s\n", device->getName().c_str());
            BLE_DEBUG_PRINTF("Address : %s\n", device->getAddress().toString().c_str());

            bool is_bonded = NimBLEDevice::isBonded(device->getAddress());

            if (device->isAdvertisingService(NimBLEUUID("1812")))
                BLE_DEBUG_PRINTLN("HID Service Found");

            bool matches = false;
            if (target_device_name.length() > 0) {
                if (strcmp(device->getName().c_str(), target_device_name.c_str()) == 0) {
                    matches = true;
                }
            }

            if (matches) {
                BLE_DEBUG_PRINTF(">>> %s FOUND <<<\n", device->getName().c_str());

                if (keyboard_enabled && !keyboard.is_connected()) {
                    BLE_DEBUG_PRINTLN("Connecting...");

                    if (keyboard.connect(device))
                        BLE_DEBUG_PRINTLN("Connected!");
                    else
                        BLE_DEBUG_PRINTLN("Connection Failed!");
                }
            }
        }
    }

    if (keyboard_enabled && auto_connect && !keyboard.is_connected()) {
        start_scan();
    }
}

void BLEHIDClient::start_scan() {
    if (last_scan == 0 || (millis() - last_scan >= BLE_HID_SCAN_PERIOD)) {
        last_scan = millis();
        BLE_DEBUG_PRINTLN("Scanning for BLE HID devices...");
        pScan->start(BLE_HID_SCAN_DURATION);
    }
}

void BLEHIDClient::enable_keyboard() {
    keyboard_enabled = true;
}

void BLEHIDClient::disable_keyboard() {
    keyboard_enabled = false;
}

BLEKeyboard& BLEHIDClient::get_keyboard() {
    return keyboard;
}
