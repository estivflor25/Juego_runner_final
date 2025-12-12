#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "Vector2D.h"
#include "GraphicsManager.h"
#include "Sprites.h"
#include <Arduino.h>
#include <TFT_eSPI.h>

// Constantes necesarias
#ifndef SCROLL_SPEED
#define SCROLL_SPEED 6.0f
#endif

#ifndef COLOR_OBSTACLE
#define COLOR_OBSTACLE TFT_BLUE
#endif

class Obstacle {
private:
    Vector2D position;
    int width;
    int height;
    bool active;
    GraphicsManager* graphics;
    
public:
    Obstacle(GraphicsManager* gfx = nullptr);
    
    void activate(float x, float y, int w, int h);
    void deactivate();
    
    bool isActive() const;
    int getWidth() const;
    int getHeight() const;
    float getX() const;
    float getY() const;
    
    void update();
    
    template<typename T>
    void draw(T* display) const {
        if (!active || !display) return;
        
        // Si es tamaño sprite estándar (16x24), dibujar sprite escalado
        if (width == 16 && height == 24) {
            int scale = 3;  
            int spriteWidth = 16;
            int spriteHeight = 24;
            
            for (int row = 0; row < spriteHeight; row++) {
                for (int col = 0; col < spriteWidth; col++) {
                    uint16_t color = OBSTACLE_SPRITE[row * spriteWidth + col];
                    if (color != 0x0000) { 
                        int x = (int)position.x + col * scale;
                        int y = (int)position.y + row * scale;
                        
                        // Dibujar bloque escalado
                        for (int sy = 0; sy < scale; sy++) {
                            for (int sx = 0; sx < scale; sx++) {
                                display->drawPixel(x + sx, y + sy, color);
                            }
                        }
                    }
                }
            }
        } else {
            //  dibujar rectángulo con color y patrón
            display->fillRect((int)position.x, (int)position.y, width, height, COLOR_OBSTACLE);
            display->drawRect((int)position.x, (int)position.y, width, height, TFT_BLACK);
            
            // Dibujar líneas para simular ventanas
            for (int i = 1; i < height; i += 8) {
                display->drawFastHLine((int)position.x, (int)position.y + i, width, TFT_BLACK);
            }
            for (int i = 4; i < width; i += 8) {
                display->drawFastVLine((int)position.x + i, (int)position.y, height, TFT_BLACK);
            }
        }
    }
    
    bool checkCollision(float px, float py, int pw, int ph) const;
    char getCollisionSide(float px, float py, int pw, int ph) const;
};

#endif