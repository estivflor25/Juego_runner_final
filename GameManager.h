#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>
#include <esp_task_wdt.h>

#include "Vector2D.h"
#include "GraphicsManager.h"
#include "Player.h"
#include "Obstacle.h"
#include "HUDManager.h"
#include "BluetoothManager.h"
#include "SoundManager.h"  


#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320
#define HUD_HEIGHT 30
#define GROUND_Y 280
#define PLAYER_WIDTH 40
#define PLAYER_HEIGHT 40
#define GRAVITY 0.8f
#define JUMP_POWER -15.0f
#define PLAYER_SCREEN_X 100.0f
#define SCROLL_SPEED 6.0f
#define DASH_SPEED 22.0f
#define SPAWN_RATE 70
#define MAX_OBSTACLES 10
#define SPRITE_ZONE_HEIGHT 180
#define SPRITE_ZONE_Y 80

#define COLOR_SKY 0x67FD
#define COLOR_GROUND 0x4A49
#define COLOR_PLAYER TFT_GREEN
#define COLOR_OBSTACLE TFT_BLUE
#define COLOR_TEXT TFT_WHITE

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif


extern SemaphoreHandle_t displayMutex;
extern SemaphoreHandle_t gameStateMutex;
extern QueueHandle_t commandQueue;

class GameManager {
public:
    TFT_eSPI display;
    TFT_eSprite sprite;
    Player player;
    Obstacle obstacles[MAX_OBSTACLES];
    int obstacleCount;
    HUDManager* hud;
    BluetoothManager* bluetooth;
    GraphicsManager graphics;
    SoundManager sound; 
    
    int spawnTimer;
    bool gameOver;
    bool paused;
    bool usePartialBuffer;
    
    GameManager();
    ~GameManager();
    
    void setup();
    
private:
    void renderInitialFrame();
    void updateGame();
    void renderGameOptimized();
    void processCommandFast(const String& cmd);
    void showGameOver();
    void resetGame();
    
    static void gameLogicTask(void* parameter);
    static void renderTask(void* parameter);
    static void inputTask(void* parameter);
};

#endif