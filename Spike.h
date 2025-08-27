#pragma once
#include "Obstacle.h"
#include <SFML/Graphics.hpp>

class Spike : public Obstacle {
private:
    sf::Texture spikeTexture;
    sf::Sprite spikeSprite;
    const float DAMAGE = 1.0f;  // Damage dealt to player when hit

public:
    Spike(float x, float y, float size) : Obstacle(x, y, size, size) {
        // Load spike texture
        if (!spikeTexture.loadFromFile("Data/spike.png")) {
            std::cout << "Failed to load spike texture" << std::endl;
        }
        spikeSprite.setTexture(spikeTexture);
        
        // Scale sprite to match cell size
        spikeSprite.setScale(
            static_cast<float>(size) / spikeTexture.getSize().x,
            static_cast<float>(size) / spikeTexture.getSize().y
        );
    }

    void update(float deltaTime) override {
        // Spikes don't need any update logic
    }

    void draw(sf::RenderWindow& window, float camera_offset_x) override {
        spikeSprite.setPosition(x - camera_offset_x, y);
        window.draw(spikeSprite);
    }

    bool checkCollision(float playerX, float playerY, float playerWidth, float playerHeight) override {
        // Simple collision check
        return (playerX < x + width &&
                playerX + playerWidth > x &&
                playerY < y + height &&
                playerY + playerHeight > y);
    }

    float getDamage() const { return DAMAGE; }
}; 