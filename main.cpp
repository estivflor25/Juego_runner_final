#include <Arduino.h>
#include "GameManager.h" 

SemaphoreHandle_t displayMutex = nullptr;
SemaphoreHandle_t gameStateMutex = nullptr;
QueueHandle_t commandQueue = nullptr;
GameManager game;

void setup() {
    game.setup();
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}