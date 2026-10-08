#include "BleKeyboardClient.h"

/* =========================================================================
 * Scan Callbacks
 * ========================================================================= */

void BLEHIDClientScanCallbacks::onResult(const NimBLEAdvertisedDevice* advertisedDevice) {
    BLE_HID_DEBUG("device found: %s (appearance = 0x%x)\n", advertisedDevice->toString().c_str(), advertisedDevice->getAppearance());
}

void BLEHIDClientScanCallbacks::onScanEnd(const NimBLEScanResults& results, int reason) {
    BLE_HID_DEBUG("scan end");
    this->results = &results;
}

/* =========================================================================
 * Client Connection Callbacks
 * ========================================================================= */

void BLEHIDClientCallbacks::onConnect(NimBLEClient* pClient) { 
    BLE_HID_DEBUG("Connected");
    connected = true;
}

void BLEHIDClientCallbacks::onDisconnect(NimBLEClient* pClient, int reason) {
    BLE_HID_DEBUG("%s Disconnected, reason = %d", pClient->getPeerAddress().toString().c_str(), reason);
    connected = false;
    NimBLEDevice::getScan()->start(BLE_HID_SCAN_DURATION);
}

/** Pairing process complete, we can check the results in connInfo */
void BLEHIDClientCallbacks::onAuthenticationComplete(NimBLEConnInfo& connInfo) {
    BLE_HID_DEBUG("onAuthenticationComplete");
    BLE_HID_DEBUG("authenticated: %d, bonded: %d, encrypted: %d, key size: %u",
        connInfo.isAuthenticated(),
        connInfo.isBonded(),
        connInfo.isEncrypted(),
        connInfo.getSecKeySize()
    );
    if (!connInfo.isEncrypted()) {
        BLE_HID_DEBUG("Encrypt connection failed - disconnecting");
        /** Find the client with the connection handle provided in connInfo */
        NimBLEDevice::getClientByHandle(connInfo.getConnHandle())->disconnect();
        return;
    }
}
