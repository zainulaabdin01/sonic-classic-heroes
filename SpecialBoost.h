#ifndef SPECIALBOOST_H
#define SPECIALBOOST_H

#include "Collectible.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>

using namespace sf;

class SpecialBoost : public Collectible {
private:
    static Texture boostTexture;
    Sprite sprite;
    float hoverTimer;
    float baseY;
    static float hoverAmplitude; // pixels
    static float hoverSpeed; // radians/sec
    static int frameWidth;
    static int frameHeight;

public:
    SpecialBoost(float startX, float startY, float scale = 2.0f)
        : Collectible(startX, startY, scale), hoverTimer(0.0f), baseY(startY) {
        width = frameWidth * scale;
        height = frameHeight * scale;
        if (boostTexture.getSize().x == 0) {
            boostTexture.loadFromFile("Data/special_boost.png");
        }
        sprite.setTexture(boostTexture);
        sprite.setTextureRect(IntRect(0, 0, frameWidth, frameHeight));
        sprite.setScale(scale, scale);
        sprite.setPosition(x, y);
    }

    void update(float deltaTime) override {
        if (!isCollected && isVisible) {
            hoverTimer += deltaTime * hoverSpeed;
            float offsetY = std::sin(hoverTimer) * hoverAmplitude;
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
        // Logic to be implemented later
    }

    bool checkCollision(float playerX, float playerY, int playerWidth, int playerHeight) override {
        if (isCollected || !isVisible) return false;
        return (playerX < x + width &&
                playerX + playerWidth > x &&
                playerY < y + height &&
                playerY + playerHeight > y);
    }

    string getType() const override {
        return "SpecialBoost";
    }
};

// Static texture definition
Texture SpecialBoost::boostTexture;

// Static variable definitions
float SpecialBoost::hoverAmplitude = 8.0f;
float SpecialBoost::hoverSpeed = 2.0f;
int SpecialBoost::frameWidth = 32;
int SpecialBoost::frameHeight = 32;

#endif // SPECIALBOOST_H 