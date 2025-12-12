#include "HUDManager.h"

HUDManager::HUDManager(TFT_eSPI* tft) : display(tft), lives(3), score(0), lastScore(-1), lastLives(-1) {}

void HUDManager::draw() {
    if (score == lastScore && lives == lastLives) return;
    
    if (xSemaphoreTake(displayMutex, pdMS_TO_TICKS(8))) {
        display->fillRect(0, 0, SCREEN_WIDTH, HUD_HEIGHT, TFT_NAVY);
        
        display->setTextColor(COLOR_TEXT);
        display->setTextSize(2);
        display->setCursor(10, 8);
        display->print("Vidas: ");
        
        for (int i = 0; i < 3; i++) {
            if (i < lives) {
                display->fillRect(100 + i * 25, 8, 20, 15, TFT_RED);
            } else {
                display->drawRect(100 + i * 25, 8, 20, 15, COLOR_TEXT);
            }
        }
        
        display->fillRect(250, 0, 230, HUD_HEIGHT, TFT_NAVY);
        display->setCursor(250, 8);
        display->print("Puntos: ");
        display->print(score);
        
        lastScore = score;
        lastLives = lives;
        
        xSemaphoreGive(displayMutex);
    }
}

void HUDManager::addScore(int p) { 
    score += p; 
}

void HUDManager::loseLife() {
    if (lives > 0) {
        lives--;
        Serial.print("💀 Vida perdida! Ahora: ");
        Serial.println(lives);
    }
}

void HUDManager::reset() { 
    lives = 3; 
    score = 0; 
    lastScore = -1; 
    lastLives = -1; 
}

bool HUDManager::isGameOver() const { 
    return lives <= 0; 
}

int HUDManager::getLives() const { 
    return lives; 
}

int HUDManager::getScore() const { 
    return score; 
}