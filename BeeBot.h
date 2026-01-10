#ifndef BEEBOT_H
#define BEEBOT_H

#include "Enemy.h"

struct Projectile {
    float x, y;
    float velX, velY;
    bool active;
    static const float SIZE;
};
const float Projectile::SIZE = 8.0f;

class BeeBot : public Enemy {
    static const float FIRE_RATE;
    static const float PROJECTILE_SPEED;
    static const int MAX_PROJECTILES;
    static const float SIZE;

    Clock fireClock;
    float patternOffset;
    Projectile projectiles[2];  // Match MAX_PROJECTILES
    Texture projectileTex;
    Sprite projectileSprite;

public:
    BeeBot(float startX, float startY) : Enemy() {
        if (!texture.loadFromFile("Data/beebot.png")) return;
        sprite.setTexture(texture);
        sprite.setScale(1.0f, 1.0f);

        posX = startX;
        posY = startY;
        health = 5;
        speed = 150.0f;
        width = SIZE;
        height = SIZE;
        patternOffset = 0.0f;

        // Projectile graphic setup
        projectileTex.loadFromFile("Data/red_pixel.png");
        projectileSprite.setTexture(projectileTex);
        projectileSprite.setScale(Projectile::SIZE, Projectile::SIZE);
        projectileSprite.setColor(Color::Red);

        for (int i = 0; i < 2; i++) {
            projectiles[i].active = false;
        }
    }

    void update(float deltaTime, float playerX, float playerY) override {
        if (!isAlive) return;

        // Sinusoidal movement pattern
        patternOffset += deltaTime;
        posY += sin(patternOffset * 3.0f) * 50.0f * deltaTime;
        posX += cos(patternOffset * 1.5f) * 25.0f * deltaTime;

        // Firing logic
        if (fireClock.getElapsedTime().asSeconds() >= FIRE_RATE) {
            fireClock.restart();

            float centerX = posX + width / 2;
            float centerY = posY + height / 2;
            float dx = playerX - centerX;
            float dy = playerY - centerY;
            float length = sqrt(dx * dx + dy * dy);

            if (length > 0) {
                for (int i = 0; i < 2; i++) {
                    if (!projectiles[i].active) {
                        projectiles[i].x = centerX - Projectile::SIZE / 2;
                        projectiles[i].y = centerY - Projectile::SIZE / 2;
                        projectiles[i].velX = (dx / length) * PROJECTILE_SPEED;
                        projectiles[i].velY = (dy / length) * PROJECTILE_SPEED;
                        projectiles[i].active = true;
                        break;
                    }
                }
            }
        }

        // Update projectiles
        for (int i = 0; i < 2; i++) {
            if (projectiles[i].active) {
                projectiles[i].x += projectiles[i].velX * deltaTime;
                projectiles[i].y += projectiles[i].velY * deltaTime;

                // Deactivate projectiles outside view
                if (projectiles[i].x < -100 || projectiles[i].x > 1300 ||
                    projectiles[i].y < -100 || projectiles[i].y > 1000) {
                    projectiles[i].active = false;
                }
            }
        }
    }

    void draw(RenderWindow& window, float camera_offset_x) override {
        Enemy::draw(window, camera_offset_x);

        // Draw active projectiles
        for (int i = 0; i < 2; i++) {
            if (projectiles[i].active) {
                projectileSprite.setPosition(
                    projectiles[i].x - camera_offset_x,
                    projectiles[i].y
                );
                window.draw(projectileSprite);
            }
        }
    }

    static int getMaxProjectiles() { return 2; }
    const Projectile& getProjectile(int idx) const { return projectiles[idx]; }
};

const float BeeBot::FIRE_RATE = 1.5f;
const float BeeBot::PROJECTILE_SPEED = 150.0f;
const int BeeBot::MAX_PROJECTILES = 2;
const float BeeBot::SIZE = 48.0f;

#endif // BEEBOT_H