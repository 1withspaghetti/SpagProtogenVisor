#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "settings.h"

// Service ID, should be the same as the client so the client knows which device to connect to.
#define SERVICE_UUID              "9d537c81-4b1f-406c-b12f-a9aa49af6333"

// Keys for the clients to update, left client should use the CHARACTERISTIC_LEFT_UUID 
// to set values and the right client should use CHARACTERISTIC_RIGHT_UUID
#define CHARACTERISTIC_LEFT_UUID  "ac61fa72-e2de-42fb-9605-d0c7549b1c39"
#define CHARACTERISTIC_RIGHT_UUID "b3f4eb92-9ceb-4317-90ed-373a36164d2b"

#define MAX_CLIENTS 2

class ProtoBLEServer {
    public:
        // These are the expected addresses of the clients, however this is only used for logging/visual purposes
        // and all the functionality is based on characteristic UUIDs. It will still work if the clients have different addresses.
        static esp_bd_addr_t leftADDR;
        static esp_bd_addr_t rightADDR;
        
        ProtoBLEServer();
        void setup();
        /**
         * Gets the last "click" state of the menu buttons.
         * Also resets the state so it only returns the new clicks since last call.
         */
        uint8_t getButtonState();

        // All the variables are public so they can be accessed from the callbacks
        int devicesConnected = 0;
        bool leftConnected = false;
        bool rightConnected = false;
        uint8_t leftValue = 0;
        uint8_t rightValue = 0;
        unsigned long lastUpdate = 0;
        uint8_t lastButtonState = 0;
    private:
        BLEServer *server;
        BLEService *service;
        BLECharacteristic *leftCharacteristic;
        BLECharacteristic *rightCharacteristic;
};

class CharacteristicChangeCallbacks: public BLECharacteristicCallbacks {
    public:
        CharacteristicChangeCallbacks(ProtoBLEServer *bt);
        void onWrite(BLECharacteristic *characteristic);
    private:
        ProtoBLEServer *bt;
};

class ServerCallbacks: public BLEServerCallbacks {
    public:
        ServerCallbacks(ProtoBLEServer *bt);
        void onConnect(BLEServer *server, esp_ble_gatts_cb_param_t *param);
        void onDisconnect(BLEServer *server, esp_ble_gatts_cb_param_t *param);
    private:
        ProtoBLEServer *bt;
    protected:
        bool leftConnected = false;
        bool rightConnected = false;
};