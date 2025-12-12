#ifndef BLUETOOTH_MANAGER_H
#define BLUETOOTH_MANAGER_H

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

// UUIDs del servicio Bluetooth
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_RX_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_TX_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

// Declaración externa de la cola de comandos
extern QueueHandle_t commandQueue;

class BluetoothManager {
private:
    NimBLEServer* server;
    NimBLECharacteristic* txCharacteristic;
    bool connected;
    static BluetoothManager* instance;
    
public:
    BluetoothManager();
    
    void init();
    bool isConnected() const;
    void setConnected(bool v);
    static BluetoothManager* getInstance();
    
    class ServerCallbacks : public NimBLEServerCallbacks {
        void onConnect(NimBLEServer* pServer) override;
        void onDisconnect(NimBLEServer* pServer) override;
    };
    
    class RxCallbacks : public NimBLECharacteristicCallbacks {
        void onWrite(NimBLECharacteristic* pCharacteristic) override;
    };
};

#endif