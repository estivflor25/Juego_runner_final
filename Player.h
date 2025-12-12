#ifndef PLAYER_H
#define PLAYER_H

#include "Vector2D.h"
#include "GraphicsManager.h"
#include "Sprites.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// Constantes del jugador
#ifndef PLAYER_SCREEN_X
#define PLAYER_SCREEN_X 100.0f
#endif

#ifndef GROUND_Y
#define GROUND_Y 280
#endif

#ifndef PLAYER_WIDTH
#define PLAYER_WIDTH 40
#endif

#ifndef PLAYER_HEIGHT
#define PLAYER_HEIGHT 40
#endif

#ifndef GRAVITY
#define GRAVITY 0.8f
#endif

#ifndef JUMP_POWER
#define JUMP_POWER -15.0f
#endif

#ifndef COLOR_PLAYER
#define COLOR_PLAYER TFT_GREEN
#endif

extern SemaphoreHandle_t gameStateMutex;

class Player {
private:
    Vector2D position;
    Vector2D velocity;
    bool onGround;
    int dashCooldown;
    GraphicsManager* graphics;
    
public:
    Player(GraphicsManager* gfx = nullptr);
    
    void update();
    
    template<typename T>
    void draw(T* display) const {
        if (!display) return;
        

        int scale = 3; 
        int spriteWidth = 16;
        int spriteHeight = 16;
        
        for (int row = 0; row < spriteHeight; row++) {
            for (int col = 0; col < spriteWidth; col++) {
                uint16_t color = PLAYER_SPRITE[row * spriteWidth + col];
                
                if (color != 0x0000) {
                    int x = (int)position.x + col * scale;
                    int y = (int)position.y + row * scale;
                    
                    for (int sy = 0; sy < scale; sy++) {
                        for (int sx = 0; sx < scale; sx++) {
                            display->drawPixel(x + sx, y + sy, color);
                        }
                    }
                }
            }
        }
        
        if (dashCooldown > 0) {
            display->drawRect((int)position.x - 1, (int)position.y - 1, 49, 49, TFT_YELLOW);
        }
    }
    
    void jump();
    void dash();
    void reset();
    
    float getScreenX() const;
    float getScreenY() const;
    int getWidth() const;
    int getHeight() const;
    int getDashCooldown() const;
    bool isOnGround() const;
};

#endif