#include "Obstacle.h"

Obstacle::Obstacle(GraphicsManager* gfx) : position(0,0), width(0), height(0), active(false), graphics(gfx) {}

void Obstacle::activate(float x, float y, int w, int h) {
    position.set(x, y);
    width = w;
    height = h;
    active = true;
}

void Obstacle::deactivate() {
    active = false;
    width = height = 0;
}

bool Obstacle::isActive() const { return active; }
int Obstacle::getWidth() const { return width; }
int Obstacle::getHeight() const { return height; }
float Obstacle::getX() const { return position.x; }
float Obstacle::getY() const { return position.y; }

void Obstacle::update() {
    if (active) position.x -= SCROLL_SPEED;
}

bool Obstacle::checkCollision(float px, float py, int pw, int ph) const {
    if (!active) return false;
    return (px < position.x + width &&
            px + pw > position.x &&
            py < position.y + height &&
            py + ph > position.y);
}

char Obstacle::getCollisionSide(float px, float py, int pw, int ph) const {
    if (!active) return 'N';
    float playerCenterX = px + pw/2.0f;
    float playerCenterY = py + ph/2.0f;
    float obsCenterX = position.x + width/2.0f;
    float obsCenterY = position.y + height/2.0f;
    float dx = playerCenterX - obsCenterX;
    float dy = playerCenterY - obsCenterY;
    float overlapX = (pw + width)/2.0f - abs(dx);
    float overlapY = (ph + height)/2.0f - abs(dy);
    if (overlapX < overlapY) return (dx > 0) ? 'R' : 'L';
    return (dy > 0) ? 'B' : 'T';
}