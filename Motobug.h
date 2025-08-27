#ifndef MOTOBUG_H
#define MOTOBUG_H

#include "Enemy.h"

class Motobug : public Enemy {
    static const float TRACK_SPEED;
    static const float ACTIVATION_RANGE;

public:
    Motobug(float startX, float startY) {
        texture.loadFromFile("Data/motobug.png");
        sprite.setTexture(texture);
        sprite.setScale(1.0, 1.0);
        posX = startX;
        posY = startY;
        health = 2;
        speed = TRACK_SPEED;
        width = 64.0f;
        height = 64.0f;
    }

    void update(float deltaTime, float playerX, float playerY) override {
        if (!isAlive) return;
        float dx = playerX - posX;
        if (fabs(dx) < ACTIVATION_RANGE) {
            if (dx > 0) posX += speed * deltaTime;
            else posX -= speed * deltaTime;
        }
    }

    void draw(RenderWindow& window, float camera_offset_x) override {
        Enemy::draw(window, camera_offset_x);
    }
};
const float Motobug::TRACK_SPEED = 60.0f;
const float Motobug::ACTIVATION_RANGE = 300.0f;

#endif // MOTOBUG_H 