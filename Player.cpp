#include "Player.h"

Player::Player(GraphicsManager* gfx) : graphics(gfx) {
    position.set(PLAYER_SCREEN_X, GROUND_Y - PLAYER_HEIGHT);
    velocity.set(0, 0);
    onGround = true;
    dashCooldown = 0;
}

void Player::update() {
    if (xSemaphoreTake(gameStateMutex, portMAX_DELAY) == pdTRUE) {
        velocity.y += GRAVITY;
        position.y += velocity.y;

        if (position.y >= GROUND_Y - PLAYER_HEIGHT) {
            position.y = GROUND_Y - PLAYER_HEIGHT;
            velocity.y = 0;
            onGround = true;
        } else {
            onGround = false;
        }

        if (dashCooldown > 0) dashCooldown--;
        xSemaphoreGive(gameStateMutex);
    }
}

void Player::jump() {
    if (xSemaphoreTake(gameStateMutex, portMAX_DELAY) == pdTRUE && onGround) {
        velocity.y = JUMP_POWER;
        onGround = false;
        Serial.println(" Salto!");
        xSemaphoreGive(gameStateMutex);
    }
}

void Player::dash() {
    if (xSemaphoreTake(gameStateMutex, portMAX_DELAY) == pdTRUE) {
        if (dashCooldown <= 0) {
            dashCooldown = 30;
            Serial.println(" DASH!");
        }
        xSemaphoreGive(gameStateMutex);
    }
}

void Player::reset() {
    if (xSemaphoreTake(gameStateMutex, portMAX_DELAY) == pdTRUE) {
        position.set(PLAYER_SCREEN_X, GROUND_Y - PLAYER_HEIGHT);
        velocity.set(0, 0);
        onGround = true;
        dashCooldown = 0;
        xSemaphoreGive(gameStateMutex);
    }
}

float Player::getScreenX() const { return position.x; }
float Player::getScreenY() const { return position.y; }
int Player::getWidth() const { return PLAYER_WIDTH; }
int Player::getHeight() const { return PLAYER_HEIGHT; }
int Player::getDashCooldown() const { return dashCooldown; }
bool Player::isOnGround() const { return onGround; }