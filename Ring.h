#ifndef RING_H
#define RING_H

#include "Collectible.h"
#include <SFML/Graphics.hpp>
#include "ScoreManager.h"

using namespace sf;
using namespace std;

class Ring : public Collectible {
private:
    static Texture ringTexture;
    Sprite sprite;
    int currentFrame;
    float frameTimer;
    static float frameDuration; // seconds per frame
    static int totalFrames;
    static int frameWidth;
    static int frameHeight;
    Music ringMusic;
    ScoreManager* scoreManager;


public:
    Ring(float startX, float startY, ScoreManager* scoreMgr, float scale = 2.0f)
        : Collectible(startX, startY, scale), currentFrame(0), frameTimer(0.0f), scoreManager(scoreMgr) {
        width = frameWidth * scale;
        height = frameHeight * scale;
        if (ringTexture.getSize().x == 0) {
            ringTexture.loadFromFile("Data/ring.png");
        }
        sprite.setTexture(ringTexture);
        sprite.setTextureRect(IntRect(0, 0, frameWidth, frameHeight));
        sprite.setScale(scale, scale);
        sprite.setPosition(x, y);
        ringMusic.openFromFile("Data/Ring.wav");
        ringMusic.setVolume(30);
    }

    void update(float deltaTime) override {
        if (!isCollected && isVisible) {
            frameTimer += deltaTime;
            if (frameTimer >= frameDuration) {
                frameTimer -= frameDuration;
                currentFrame = (currentFrame + 1) % totalFrames;
                sprite.setTextureRect(IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
            }
            sprite.setPosition(x, y);
        }
    }

    void draw(RenderWindow& window, float camera_offset_x) override {
        if (!isCollected && isVisible) {
            sprite.setPosition(x - camera_offset_x, y);
            window.draw(sprite);
        }
    }

    void onCollect() override {
        isCollected = true;
        isVisible = false;
        ringMusic.play();
        if (scoreManager) scoreManager->addScore(10);
    }

    bool checkCollision(float playerX, float playerY, int playerWidth, int playerHeight) override {
        if (isCollected || !isVisible) return false;

        // Use the same AABB collision logic as Spike
        return (playerX < x + width &&
                playerX + playerWidth > x &&
                playerY < y + height &&
                playerY + playerHeight > y);
    }

    string getType() const override {
        return "Ring";
    }
};

// Static texture definition
Texture Ring::ringTexture;

// Static variable definitions
float Ring::frameDuration = 0.1f;
int Ring::totalFrames = 4;
int Ring::frameWidth = 16;
int Ring::frameHeight = 16;

#endif // RING_H 