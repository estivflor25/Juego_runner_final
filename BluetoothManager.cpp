#include "BluetoothManager.h"

// Definición de la variable estática
BluetoothManager* BluetoothManager::instance = nullptr;

BluetoothManager::BluetoothManager() : server(nullptr), txCharacteristic(nullptr), connected(false) { 
    instance = this; 
}

void BluetoothManager::init() {
    NimBLEDevice::init("ESP32_CyberRunner");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    server = NimBLEDevice::createServer();
    server->setCallbacks(new ServerCallbacks());
    NimBLEService* service = server->createService(SERVICE_UUID);
    
    txCharacteristic = service->createCharacteristic(
        CHARACTERISTIC_TX_UUID, 
        NIMBLE_PROPERTY::NOTIFY
    );
    
    NimBLECharacteristic* rxCharacteristic = service->createCharacteristic(
        CHARACTERISTIC_RX_UUID, 
        NIMBLE_PROPERTY::WRITE
    );
    
    rxCharacteristic->setCallbacks(new RxCallbacks());
    service->start();
    NimBLEDevice::getAdvertising()->addServiceUUID(SERVICE_UUID);
    NimBLEDevice::getAdvertising()->start();
    Serial.println(" Bluetooth iniciado: ESP32_CyberRunner");
}

bool BluetoothManager::isConnected() const { 
    return connected; 
}

void BluetoothManager::setConnected(bool v) { 
    connected = v; 
}

BluetoothManager* BluetoothManager::getInstance() { 
    return instance; 
}


void BluetoothManager::ServerCallbacks::onConnect(NimBLEServer* pServer) {
    if (BluetoothManager::getInstance()) {
        BluetoothManager::getInstance()->setConnected(true);
        Serial.println(" MicroBlue CONECTADO!");
        digitalWrite(LED_BUILTIN, HIGH);
    }
}

void BluetoothManager::ServerCallbacks::onDisconnect(NimBLEServer* pServer) {
    if (BluetoothManager::getInstance()) {
        BluetoothManager::getInstance()->setConnected(false);
        Serial.println(" MicroBlue DESCONECTADO");
        digitalWrite(LED_BUILTIN, LOW);
        NimBLEDevice::startAdvertising();
    }
}

void BluetoothManager::RxCallbacks::onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0) {
        String cmd = String(value.c_str());
        cmd.trim();
        String cleanCmd = "";
        for (int i = 0; i < cmd.length(); i++) {
            char c = cmd.charAt(i);
            if (c >= 32 && c <= 126) cleanCmd += c;
        }
        if (cleanCmd.length() > 0) {
            char* buf = (char*)malloc(cleanCmd.length() + 1);
            if (buf) {
                strcpy(buf, cleanCmd.c_str());
                xQueueSend(commandQueue, &buf, 0);
            }
        }
    }
}