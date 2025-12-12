#include "GraphicsManager.h"
#include "Sprites.h"

GraphicsManager::GraphicsManager(TFT_eSPI* tft) 
    : display(tft), currentFrame(0), animationTimer(0) {
}

void GraphicsManager::drawRGBSprite(int x, int y, const uint16_t* sprite, int width, int height) {
    if (!display || !sprite) return;
    
    display->startWrite();
    for (int row = 0; row < height; row++) {
        int screenY = y + row;
        if (screenY < 0 || screenY >= display->height()) continue;
        
        for (int col = 0; col < width; col++) {
            int screenX = x + col;
            if (screenX < 0 || screenX >= display->width()) continue;
            
            uint16_t color = sprite[row * width + col];
            if (color != 0x0000) { 
                display->drawPixel(screenX, screenY, color);
            }
        }
    }
    display->endWrite();
}

void GraphicsManager::drawRGBSprite(TFT_eSprite* spriteBuffer, int x, int y, const uint16_t* sprite, int width, int height) {
    if (!spriteBuffer || !sprite) return;
    
    for (int row = 0; row < height; row++) {
        int bufY = y + row;
        if (bufY < 0 || bufY >= spriteBuffer->height()) continue;
        
        for (int col = 0; col < width; col++) {
            int bufX = x + col;
            if (bufX < 0 || bufX >= spriteBuffer->width()) continue;
            
            uint16_t color = sprite[row * width + col];
            if (color != 0x0000) { 
                spriteBuffer->drawPixel(bufX, bufY, color);
            }
        }
    }
}

void GraphicsManager::drawPlayer(int x, int y, bool isDashing) {
    if (!display) return;
    

    drawRGBSprite(x, y, PLAYER_SPRITE, 16, 16);
}

void GraphicsManager::drawPlayer(TFT_eSprite* spriteBuffer, int x, int y, bool isDashing) {
    if (!spriteBuffer) return;
    

    drawRGBSprite(spriteBuffer, x, y, PLAYER_SPRITE, 16, 16);
}

void GraphicsManager::drawObstacle(int x, int y, int width, int height) {
    if (!display) return;
    

    if (width == 16 && height == 24) {
        drawRGBSprite(x, y, OBSTACLE_SPRITE, 16, 24);
    } else {
        display->fillRect(x, y, width, height, TFT_BLUE);
    }
}

void GraphicsManager::drawObstacle(TFT_eSprite* spriteBuffer, int x, int y, int width, int height) {
    if (!spriteBuffer) return;
    
   
    if (width == 16 && height == 24) {
        drawRGBSprite(spriteBuffer, x, y, OBSTACLE_SPRITE, 16, 24);
    } else {
        spriteBuffer->fillRect(x, y, width, height, TFT_BLUE);
    }
}

void GraphicsManager::drawBackground(TFT_eSprite* sprite) {
    if (!sprite) return;
    

    sprite->fillScreen(0x67FD);  
}

void GraphicsManager::updateAnimation() {
    animationTimer++;
    if (animationTimer >= 8) {
        animationTimer = 0;
        currentFrame = (currentFrame + 1) % 4;
    }
}