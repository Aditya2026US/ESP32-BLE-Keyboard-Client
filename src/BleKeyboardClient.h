#ifndef BLE_KEYBOARD_CLIENT_H
#define BLE_KEYBOARD_CLIENT_H

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <functional>

#define BLE_HID_SCAN_DURATION 5000
#define BLE_HID_SCAN_PERIOD 10000

extern bool g_ble_hid_debug_enabled;

#define BLE_DEBUG_PRINTF(...) do { if (g_ble_hid_debug_enabled) { Serial.printf(__VA_ARGS__); } } while(0)
#define BLE_DEBUG_PRINTLN(...) do { if (g_ble_hid_debug_enabled) { Serial.println(__VA_ARGS__); } } while(0)
#define BLE_HID_DEBUG(...) BLE_DEBUG_PRINTF(__VA_ARGS__)

class BLEHIDClient;

class BLEHIDClientScanCallbacks : public NimBLEScanCallbacks {
public:
    const NimBLEScanResults* results;
private:
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override;
    void onScanEnd(const NimBLEScanResults& results, int reason) override;
};

class BLEHIDClientCallbacks : public NimBLEClientCallbacks {
public:
    bool connected = false;
private:
    void onConnect(NimBLEClient* pClient) override;
    void onDisconnect(NimBLEClient* pClient, int reason) override;
    void onAuthenticationComplete(NimBLEConnInfo& connInfo) override;
};

class BLEHIDDevice {
public:
    bool is_connected();
protected:
    virtual void handle_report(uint8_t *report, size_t len) = 0;
    virtual bool connect(const NimBLEAdvertisedDevice* advDevice);
    NimBLERemoteService* pSvc = nullptr;
private:
    NimBLEAdvertisedDevice *device = nullptr;
    NimBLEClient *pClient = nullptr;
    bool enabled = false;
    friend class BLEHIDClient;
    BLEHIDClientCallbacks callbacks;
};

class BLEKeyboard : public BLEHIDDevice {
public:
    void on_key_pressed(std::function<void (bool, uint8_t)> callback);
    void on_key_released(std::function<void (bool, uint8_t)> callback);
    void debug();
    uint8_t get_modifiers() const { return modifiers_states; }

private:
    void handle_report(uint8_t *report, size_t len) override;
    bool connect(const NimBLEAdvertisedDevice* advDevice) override;
    std::function<void (bool, uint8_t)> key_pressed_callback;
    std::function<void (bool, uint8_t)> key_released_callback;
    bool keys_states[256] = {false};
    uint8_t modifiers_states = 0;
    friend class BLEHIDClient;
};

class BLEHIDClient {
public:
    void begin(const char *device_name = "ESP32 BLE HID", bool keyboard_enabled = true, bool mouse_enabled = false);
    void begin(const char *device_name, const char *target_keyboard_name);
    void loop();

    void enable_keyboard();
    void disable_keyboard();
    BLEKeyboard& get_keyboard();

    void setTargetDeviceName(const char* name);
    const char* getTargetDeviceName() const;

    bool connect(const NimBLEAdvertisedDevice* device);
    bool is_connected();

    void setAutoConnect(bool enable);
    bool isAutoConnect() const;

    void setDebug(bool enable);
    bool isDebug() const;

private:
    NimBLEScan *pScan = nullptr;
    BLEHIDClientScanCallbacks scan_callbacks;
    void start_scan();
    unsigned long last_scan = 0;

    bool keyboard_enabled = false;
    BLEKeyboard keyboard;
    String target_device_name = "";
    bool auto_connect = true;
};

// Convenient alias matching library name
typedef BLEHIDClient BleKeyboardClient;

#endif // BLE_KEYBOARD_CLIENT_H
