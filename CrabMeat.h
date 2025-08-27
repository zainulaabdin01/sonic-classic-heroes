#ifndef CRABMEAT_H
#define CRABMEAT_H

#include "Enemy.h"

class CrabMeat : public Enemy {
    static const float PATROL_SPEED;
    static const float FIRE_RATE;
    static const float PROJECTILE_SPEED;
    static const float PATROL_RANGE;

    Clock fireClock;
    float originalX;
    float patrolOffset;
    bool movingRight;
    Texture projTex;  // Simple texture for projectiles

public:
    struct Projectile {
        float x, y;
        float velX, velY;
        bool active;
    };
    Projectile projectiles[4];

    CrabMeat(float startX, float startY) : Enemy() {
        texture.loadFromFile("Data/Crab.png");
        sprite.setTexture(texture);

        // Create simple projectile texture (1x1 white pixel)
        projTex.loadFromFile("Data/white_pixel.png");  // Ensure this file exists
        posX = startX;
        posY = startY;
        health = 4;
        speed = PATROL_SPEED;
        width = 80.0f;
        height = 56.0f;
        movingRight = true;
        originalX = startX;

        for (int i = 0; i < 4; i++) {
            projectiles[i].active = false;
        }
    }

    void update(float deltaTime, float playerX, float playerY) override {
        if (!isAlive) return;

        // Patrol logic (same as original)
        float moveAmount = speed * deltaTime;
        patrolOffset += movingRight ? moveAmount : -moveAmount;
        if (patrolOffset > PATROL_RANGE) {
            movingRight = false;
            patrolOffset = PATROL_RANGE;
        }
        else if (patrolOffset < 0) {
            movingRight = true;
            patrolOffset = 0;
        }
        posX = originalX + patrolOffset;

        // Shooting logic (same as original)
        if (fireClock.getElapsedTime().asSeconds() >= FIRE_RATE) {
            fireClock.restart();
            float dx = playerX - posX;
            float dy = playerY - posY;
            float length = sqrt(dx * dx + dy * dy);

            if (length > 0) {
                for (int i = 0; i < 4; i++) {
                    if (!projectiles[i].active) {
                        projectiles[i].x = posX + width / 2;
                        projectiles[i].y = posY + height / 2;
                        projectiles[i].velX = (dx / length) * PROJECTILE_SPEED;
                        projectiles[i].velY = (dy / length) * PROJECTILE_SPEED;
                        projectiles[i].active = true;
                        break;
                    }
                }
            }
        }

        // Projectile movement (same as original)
        for (int i = 0; i < 4; i++) {
            if (projectiles[i].active) {
                projectiles[i].x += projectiles[i].velX * deltaTime;
                projectiles[i].y += projectiles[i].velY * deltaTime;
                if (projectiles[i].x > posX + 1000.0f) {
                    projectiles[i].active = false;
                }
            }
        }
    }

    void draw(RenderWindow& window, float camera_offset_x) override {
        Enemy::draw(window, camera_offset_x);

        // Draw projectiles as simple rectangles using the texture
        Sprite projSprite(projTex);
        projSprite.setColor(Color::Yellow);
        projSprite.setScale(10, 6);  // 10x6 pixels

        for (int i = 0; i < 4; i++) {
            if (projectiles[i].active) {
                projSprite.setPosition(
                    projectiles[i].x - camera_offset_x,
                    projectiles[i].y
                );
                window.draw(projSprite);
            }
        }
    }

    const Projectile& getProjectile(int idx) const { return projectiles[idx]; }
    void setProjectileActive(int idx, bool active) { projectiles[idx].active = active; }
};

const float CrabMeat::PATROL_SPEED = 90.0f;
const float CrabMeat::FIRE_RATE = 4.0f;
const float CrabMeat::PROJECTILE_SPEED = 350.0f;
const float CrabMeat::PATROL_RANGE = 270.0f;

#endif // CRABMEAT_H