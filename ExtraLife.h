#ifndef EXTRALIFE_H
#define EXTRALIFE_H

#include "Collectible.h"
#include <SFML/Graphics.hpp>
#include "ScoreManager.h"
#include "HealthManager.h"
#include <cmath>

using namespace std;
using namespace sf;
class Player;

class ExtraLife : public Collectible {
private:
    static Texture extraLifeTexture;
    Sprite sprite;
    HealthManager* healthManager;
    float hoverTimer;
    float baseY;
    static float hoverAmplitude; // pixels
    static float hoverSpeed; // radians/sec

public:
    ExtraLife(float startX, float startY, HealthManager* healthMgr, float scale = 2.0f)
        : Collectible(startX, startY, scale), healthManager(healthMgr), hoverTimer(0.0f), baseY(startY) {
        width = 16 * scale;
        height = 16 * scale;
        if (extraLifeTexture.getSize().x == 0) {
            extraLifeTexture.loadFromFile("Data/extralife.png");
        }
        sprite.setTexture(extraLifeTexture);
        sprite.setScale(scale, scale);
        sprite.setPosition(x, y);
    }

    void update(float deltaTime) override {
        if (!isCollected && isVisible) {
            hoverTimer += deltaTime * hoverSpeed;
            float offsetY = sin(hoverTimer) * hoverAmplitude;
            sprite.setPosition(x, baseY + offsetY);
        }
    }

    void draw(RenderWindow& window, float camera_offset_x) override {
        if (!isCollected && isVisible) {
            sprite.setPosition(x - camera_offset_x, sprite.getPosition().y);
            window.draw(sprite);
        }
    }

    void onCollect() override {
        isCollected = true;
        isVisible = false;
        if (healthManager) healthManager->incrementHealth();
    }

    bool checkCollision(float playerX, float playerY, int playerWidth, int playerHeight) override {
        if (isCollected || !isVisible) return false;
        return (playerX < x + width &&
                playerX + playerWidth > x &&
                playerY < y + height &&
                playerY + playerHeight > y);
    }

    std::string getType() const override {
        return "ExtraLife";
    }
};

// Static texture definition
Texture ExtraLife::extraLifeTexture;

// Static variable definitions
float ExtraLife::hoverAmplitude = 8.0f;
float ExtraLife::hoverSpeed = 2.0f;

#endif // EXTRALIFE_H 