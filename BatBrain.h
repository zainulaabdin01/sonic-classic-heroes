#ifndef BATBRAIN_H
#define BATBRAIN_H

#include "Enemy.h"

class BatBrain : public Enemy {
    static const float TRACK_SPEED;
    static const float SIZE;

public:
    BatBrain(float startX, float startY) {
        if (!loadTexture("Data/batbrain.png")) return;
        sprite.setTexture(texture);
        sprite.setScale(2.0, 2.0);
        posX = startX;
        posY = startY;
        health = 3;
        speed = TRACK_SPEED;
        width = SIZE;
        height = SIZE;
    }

    void update(float deltaTime, float playerX, float playerY) override {
        if (!isAlive) return;

        float centerX = posX + width / 2;
        float centerY = posY + height / 2;
        float dx = playerX - centerX;
        float dy = playerY - centerY;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance > 10.0f) {
            posX += (dx / distance) * speed * deltaTime;
            posY += (dy / distance) * speed * deltaTime;
        }
    }

    void draw(RenderWindow& window, float camera_offset_x) override {
        Enemy::draw(window, camera_offset_x);
    }
};
const float BatBrain::TRACK_SPEED = 80.0f;
const float BatBrain::SIZE = 64.0f;

#endif // BATBRAIN_H 