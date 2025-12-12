#ifndef HUD_MANAGER_H
#define HUD_MANAGER_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// Constantes del HUD
#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 480
#endif

#ifndef HUD_HEIGHT
#define HUD_HEIGHT 30
#endif

#ifndef COLOR_TEXT
#define COLOR_TEXT TFT_WHITE
#endif

// Declaración externa del mutex
extern SemaphoreHandle_t displayMutex;

class HUDManager {
private:
    TFT_eSPI* display;
    int lives;
    int score;
    int lastScore;
    int lastLives;
    
public:
    HUDManager(TFT_eSPI* tft);
    
    void draw();
    void addScore(int p);
    void loseLife();
    void reset();
    
    bool isGameOver() const;
    int getLives() const;
    int getScore() const;
};

#endif