#ifndef GRAPHICS_MANAGER_H
#define GRAPHICS_MANAGER_H

#include <Arduino.h>
#include <TFT_eSPI.h>

#ifndef COLOR_SKY
#define COLOR_SKY 0x67FD
#endif

#ifndef COLOR_OBSTACLE
#define COLOR_OBSTACLE TFT_BLUE
#endif

#ifndef COLOR_PLAYER
#define COLOR_PLAYER TFT_GREEN
#endif

class GraphicsManager {
private:
    TFT_eSPI* display;
    int currentFrame;
    int animationTimer;
    
public:
    GraphicsManager(TFT_eSPI* tft);
    
    void drawRGBSprite(int x, int y, const uint16_t* sprite, int width, int height);
    void drawRGBSprite(TFT_eSprite* spriteBuffer, int x, int y, const uint16_t* sprite, int width, int height);
    
    void drawPlayer(int x, int y, bool isDashing = false);
    void drawPlayer(TFT_eSprite* spriteBuffer, int x, int y, bool isDashing = false);
    
    void drawObstacle(int x, int y, int width, int height);
    void drawObstacle(TFT_eSprite* spriteBuffer, int x, int y, int width, int height);
    
    void drawBackground(TFT_eSprite* sprite);
    void updateAnimation();
};

#endif