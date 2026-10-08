#include <Arduino.h>
#include "BleKeyboardClient.h"

bool BLEHIDDevice::connect(const NimBLEAdvertisedDevice* advDevice) {
    pSvc = nullptr;
    bool characteristic_found = false;

    pClient = nullptr;

    BLE_DEBUG_PRINTLN("===== BLE CONNECT START =====");

    if (NimBLEDevice::getCreatedClientCount()) {
        BLE_DEBUG_PRINTLN("Reusing existing client...");

        pClient = NimBLEDevice::getClientByPeerAddress(advDevice->getAddress());

        if (pClient) {
            BLE_DEBUG_PRINTLN("Known client found");

            if (!pClient->connect(advDevice, false)) {
                BLE_DEBUG_PRINTLN("Reconnect FAILED");
                return false;
            }
        } else {
            BLE_DEBUG_PRINTLN("Using disconnected client");
            pClient = NimBLEDevice::getDisconnectedClient();
        }
    }

    if (!pClient) {

        BLE_DEBUG_PRINTLN("Creating new client");

        if (NimBLEDevice::getCreatedClientCount() >= NIMBLE_MAX_CONNECTIONS) {
            BLE_DEBUG_PRINTLN("Too many clients");
            return false;
        }

        pClient = NimBLEDevice::createClient();

        pClient->setClientCallbacks(&callbacks, false);
        pClient->setConnectionParams(12, 12, 0, 150);
        pClient->setConnectTimeout(5000);

        BLE_DEBUG_PRINTLN("Connecting...");

        if (!pClient->connect(advDevice)) {
            BLE_DEBUG_PRINTLN("Connection FAILED");
            goto cleanup1;
        }

        BLE_DEBUG_PRINTLN("Connected");
    }

    if (!pClient->isConnected()) {

        BLE_DEBUG_PRINTLN("Client not connected, reconnecting...");

        if (!pClient->connect(advDevice)) {
            BLE_DEBUG_PRINTLN("Reconnect FAILED");
            goto cleanup1;
        }
    }

    BLE_DEBUG_PRINTLN("Enabling secure connection...");

    if (!pClient->secureConnection()) {
        BLE_DEBUG_PRINTLN("Secure connection FAILED");
        goto cleanup2;
    }

    BLE_DEBUG_PRINTLN("Secure connection OK");

    pSvc = pClient->getService("1812");

    if (!pSvc) {
        BLE_DEBUG_PRINTLN("HID service NOT found");
        goto cleanup2;
    }

    BLE_DEBUG_PRINTLN("HID service FOUND");

    for (auto *chr : pSvc->getCharacteristics(true)) {

        BLE_DEBUG_PRINTF("Characteristic: %s\n",
                         chr->getUUID().toString().c_str());

        if (chr->getUUID() == NimBLEUUID("2a4d")) {

            BLE_DEBUG_PRINTLN("Found REPORT characteristic");

            if (chr->canNotify())
                BLE_DEBUG_PRINTLN("Supports notifications");
            else
                BLE_DEBUG_PRINTLN("NO notifications");

            if (chr->canNotify()) {

                bool ok = chr->subscribe(
                    true,
                    [this](NimBLERemoteCharacteristic *pRemoteCharacteristic,
                           uint8_t *pData,
                           size_t length,
                           bool isNotify) {

                        this->handle_report(pData, length);
                    });

                if (ok)
                    BLE_DEBUG_PRINTLN("Subscribe SUCCESS");
                else
                    BLE_DEBUG_PRINTLN("Subscribe FAILED");

                characteristic_found = true;
            }
        }
    }

    if (!characteristic_found) {
        BLE_DEBUG_PRINTLN("No REPORT characteristic found");
        goto cleanup2;
    }

    BLE_DEBUG_PRINTLN("===== BLE CONNECT SUCCESS =====");

    return true;

cleanup2:
    pClient->disconnect();

cleanup1:
    NimBLEDevice::deleteClient(pClient);
    pClient = nullptr;

    BLE_DEBUG_PRINTLN("===== BLE CONNECT FAILED =====");

    return false;
}

bool BLEHIDDevice::is_connected() {
    return callbacks.connected;
}
