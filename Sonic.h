#ifndef SONIC_H
#define SONIC_H

#include "Player.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

using namespace sf;
using namespace std;

class Sonic : public Player
{
private:
    float originalSpeed;
    // Single frame textures for different states
    Texture standingTexture;
    Texture runningTexture;
    Texture ballTexture;
    Texture currentTexture;
    
    // Simple state tracking
    bool facingRight;
    bool isBall;
    
    bool loadTexture(Texture& texture, const string& filename) {
        if (!texture.loadFromFile(filename)) {
            cout << "Failed to load texture: " << filename << endl;
            return false;
        }
        cout << "Successfully loaded texture: " << filename << endl;
        return true;
    }
    
    void updateSprite() {
        Texture* newTexture = nullptr;
        
        // Check for max speed (ball form) - priority condition
        if (abs(velocityX) >= max_speed) {
            newTexture = &ballTexture;
            isBall = true;
        }
        // Check for running
        else if (abs(velocityX) > 0) {
            newTexture = &runningTexture;
            isBall = false;
        }
        // Standing still
        else {
            newTexture = &standingTexture;
            isBall = false;
        }

        // Update texture
        currentTexture = *newTexture;
        sprite.setTexture(currentTexture);

        // Update facing direction based on velocity
        if (velocityX > 0) {
            facingRight = true;
        }
        else if (velocityX < 0) {
            facingRight = false;
        }

        // Set sprite direction and position
        if (facingRight) {
            sprite.setScale(scale_x, scale_y);
            sprite.setOrigin(0, 0);
        } else {
            sprite.setScale(-scale_x, scale_y);
            sprite.setOrigin(40, 0);
        }

        // Update main character direction for other characters
        updateMainCharacterDirection(facingRight);
    }

public:
    Sonic(float start_x, float start_y, HealthManager* healthMgr, float scale = 2.5f) : Player(start_x, start_y, healthMgr, scale) 
    {
        // Load single frame textures
        if (!loadTexture(standingTexture, "Data/sonic_standing.png")) {
            cout << "Error: Could not load standing texture!" << endl;
        }
        if (!loadTexture(runningTexture, "Data/sonic_running.png")) {
            cout << "Error: Could not load running texture!" << endl;
        }
        if (!loadTexture(ballTexture, "Data/sonic_ball.png")) {
            cout << "Error: Could not load ball texture!" << endl;
        }
        
        // Set initial texture and position
        currentTexture = standingTexture;
        sprite.setTexture(currentTexture);
        sprite.setScale(scale_x, scale_y);
        sprite.setOrigin(0, 0);  // Set initial origin
        
        // Calculate hitbox factors
        const int raw_img_x = 24;
        const int raw_img_y = 35;
        Pheight = static_cast<int>(raw_img_y * scale_y);
        Pwidth = static_cast<int>(raw_img_x * scale_x);
        hit_box_factor_x = static_cast<int>(8 * scale_x);
        hit_box_factor_y = static_cast<int>(5 * scale_y);
        
        // Initialize state
        velocityX = 0;
        velocityY = 0;
        onGround = false;
        originalSpeed = max_speed;
        facingRight = true;
        isBall = false;
    }

    void handleInput(Level* level) override
    {
        Player::handleInput(level);
        
        // Update facing direction
        if (velocityX > 0) {
            if (!facingRight) {
                facingRight = true;
            }
        }
        else if (velocityX < 0) {
            if (facingRight) {
                facingRight = false;
            }
        }
        
        if (Keyboard::isKeyPressed(Keyboard::LControl))
        {
            activateAbility(level);
        }
    }

    void updatePhysics(Level* level) override
    {
        Player::updatePhysics(level);
        updateSprite();
    }

    void activateAbility(Level* level) override
    {
        if (abilityCooldown <= 0 && !abilityActive) 
        {
            // Speed boost ability
            originalSpeed = max_speed;  // Store current speed before boost
            max_speed = originalSpeed * 3.0f;
            abilityDuration = 15.0f;
            abilityCooldown = 10.0f;
            abilityActive = true;
        }
    }
    void updateAbility(float deltaTime) override 
    {
        if (abilityActive) 
        {
            abilityDuration -= deltaTime;
            if (abilityDuration <= 0) 
            {
                max_speed = originalSpeed;  // Restore original speed
                abilityActive = false;
            }
        }
        abilityCooldown = max(0.0f, abilityCooldown - deltaTime);
    }

    float getMaxSpeed() override { return 18.0f; }
    float getJumpStrength() const override { return -20.0f; }
};

#endif // SONIC_H 