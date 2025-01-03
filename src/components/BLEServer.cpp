#include "BLEServer.h"

esp_bd_addr_t ProtoBLEServer::leftADDR = {0x64, 0xE8, 0x33, 0x00, 0xFC, 0x3E};
esp_bd_addr_t ProtoBLEServer::rightADDR = {0x64, 0xE8, 0x33, 0x84, 0x54, 0xBA};

ProtoBLEServer::ProtoBLEServer() {}

void ProtoBLEServer::setup() {
    #ifdef VERBOSE
    Serial.println("BLE: Starting server...");
    #endif

    BLEDevice::init("Spag Proto Server :3");
    server = BLEDevice::createServer();
    server->setCallbacks(new ServerCallbacks(this));

    service = server->createService(SERVICE_UUID);

    leftCharacteristic = service->createCharacteristic(
        CHARACTERISTIC_LEFT_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE
    );

    rightCharacteristic = service->createCharacteristic(
        CHARACTERISTIC_RIGHT_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE
    );

    // Set the initial values
    leftCharacteristic->setValue(&leftValue, 1);
    rightCharacteristic->setValue(&rightValue, 1);

    // Set the callbacks
    leftCharacteristic->setCallbacks(new CharacteristicChangeCallbacks(this));
    rightCharacteristic->setCallbacks(new CharacteristicChangeCallbacks(this));

    service->start();
    #ifdef VERBOSE
    Serial.println("BLE: Server started.");
    #endif

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    #ifdef VERBOSE
    Serial.println("BLE: Advertising started.");
    #endif
}

uint8_t ProtoBLEServer::getButtonState() {
    uint8_t state = lastButtonState;
    lastButtonState = 0;
    return state;
}

ServerCallbacks::ServerCallbacks(ProtoBLEServer *bt) {
    this->bt = bt;
}

void ServerCallbacks::onConnect(BLEServer *server, esp_ble_gatts_cb_param_t *param) {
    BLEAddress address = BLEAddress(param->connect.remote_bda);
    #ifdef VERBOSE_BLE
    Serial.print("BLE: Client connected | ");
    Serial.print("Addr: ");
    Serial.print(address.toString().c_str());
    #endif
    bt->devicesConnected++;
    if (address.equals(BLEAddress(bt->leftADDR))) {
        bt->leftConnected = true;
        #ifdef VERBOSE_BLE
        Serial.println(" | Left");
        #endif
    } else if (address.equals(BLEAddress(bt->leftADDR))) {
        bt->rightConnected = true;
        #ifdef VERBOSE_BLE
        Serial.println(" | Right");
        #endif
    } else {
        #ifdef VERBOSE_BLE
        Serial.println(" | Unknown");
        #endif
    }

    #ifdef VERBOSE_BLE
    Serial.print("BLE: Connected devices: ");
    Serial.println(bt->devicesConnected);
    #endif

    if (bt->devicesConnected < MAX_CLIENTS) {
        BLEDevice::startAdvertising();
    }
}

void ServerCallbacks::onDisconnect(BLEServer *server, esp_ble_gatts_cb_param_t *param) {
    BLEAddress address = BLEAddress(param->connect.remote_bda);
    #ifdef VERBOSE_BLE
    Serial.print("BLE: Client disconnected | ");
    Serial.print("Addr: ");
    Serial.print(address.toString().c_str());
    #endif
    bt->devicesConnected--;
    if (address.equals(BLEAddress(bt->leftADDR))) {
        bt->leftConnected = false;
        #ifdef VERBOSE_BLE
        Serial.println(" | Left");
        #endif
    } else if (address.equals(BLEAddress(bt->leftADDR))) {
        bt->rightConnected = false;
        #ifdef VERBOSE_BLE
        Serial.println(" | Right");
        #endif
    } else {
        #ifdef VERBOSE_BLE
        Serial.println(" | Unknown");
        #endif
    }

    #ifdef VERBOSE_BLE
    Serial.print("BLE: Connected devices: ");
    Serial.println(bt->devicesConnected);
    #endif

    if (bt->devicesConnected < MAX_CLIENTS) {
        BLEDevice::startAdvertising();
    }
}

CharacteristicChangeCallbacks::CharacteristicChangeCallbacks(ProtoBLEServer *bt) {
    this->bt = bt;
}

void CharacteristicChangeCallbacks::onWrite(BLECharacteristic *characteristic) {
    #ifdef VERBOSE_BLE
    // Serial.print("BLE: Characteristic ");
    // Serial.print(characteristic->getUUID().toString().c_str());
    // Serial.print(" updated to ");
    // Serial.println(characteristic->getValue()[0], BIN);
    #endif

    uint8_t newLeftValue = bt->leftValue;
    uint8_t newRightValue = bt->rightValue;
    if (characteristic->getUUID().equals(BLEUUID(CHARACTERISTIC_LEFT_UUID))) {
        newLeftValue = characteristic->getValue()[0];
    } else if (characteristic->getUUID().equals(BLEUUID(CHARACTERISTIC_RIGHT_UUID))) {
        newRightValue = characteristic->getValue()[0];
    }

    if (newLeftValue == bt->leftValue && newRightValue == bt->rightValue) return; // No change

    // Serial.print((newLeftValue & bt->leftValue & 0b100) != 0 && (newRightValue & bt->rightValue & 0b100) != 0);
    // Serial.print(" ");
    // Serial.print((newLeftValue & 0b011) == 0 && (newRightValue & 0b011) == 0);
    // Serial.print(" ");
    // Serial.print((bt->leftValue & 0b1) + (bt->leftValue >> 1 & 0b1) + (bt->rightValue & 0b1) + (bt->rightValue >> 1 & 0b1) == 1);
    // Serial.print(" ");
    // Serial.print(millis() - bt->lastUpdate > 100 && millis() - bt->lastUpdate < 2000);
    // Serial.print(" left: ");
    // Serial.print(bt->leftValue | 0b1000, BIN);
    // Serial.print(" newLeft: ");
    // Serial.print(newLeftValue | 0b1000, BIN);
    // Serial.print(" right: ");
    // Serial.print(bt->rightValue | 0b1000, BIN);
    // Serial.print(" newRight: ");
    // Serial.println(newRightValue | 0b1000, BIN);


    // Basically, to "click" a button, middle finger has to be down, and then put down and release one of the other fingers to click that "button"
    
    if ((newLeftValue & bt->leftValue & 0b100) != 0 && (newRightValue & bt->rightValue & 0b100) != 0 && // If the first bit is set (middle finger down) before and after on both
        (newLeftValue & 0b011) == 0 && (newRightValue & 0b011) == 0 && // Now none of the other bits should be set
        (bt->leftValue & 0b1) + (bt->leftValue >> 1 & 0b1) + (bt->rightValue & 0b1) + (bt->rightValue >> 1 & 0b1) == 1 && // And ONLY ONE of the other bits should have been set before
        millis() - bt->lastUpdate > 100 && // Held for at least 100ms
        millis() - bt->lastUpdate < 2000 // But not more than 2s
    ) {
        bt->lastButtonState = 
            (bt->leftValue >> 1 & 0b1) << 3 | // left thumb (would be button 1 on rf)
            (bt->leftValue >> 0 & 0b1) << 1 | // left index (would be button 3 on rf)
            (bt->rightValue >> 1 & 0b1) << 2 | // right thumb (would be button 2 on rf)
            (bt->rightValue >> 0 & 0b1) << 0; // right index (would be button 4 on rf)
    }

    bt->leftValue = newLeftValue;
    bt->rightValue = newRightValue;
    bt->lastUpdate = millis();
}