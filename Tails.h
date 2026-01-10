#ifndef TAILS_H
#define TAILS_H

#include "Player.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

using namespace sf;
using namespace std;

class Tails : public Player
{
private:
    float flightTimeRemaining;
    bool isFlying;
    const float FLIGHT_LIFT = -12.0f;
    const float FLIGHT_MAX_TIME = 7.0f;
    const float FLIGHT_RECHARGE_RATE = 2.0f;
    const float FLIGHT_TARGET_HEIGHT = 200.0f;
    const float FLIGHT_DESCENT_SPEED = 2.0f;
    float initialFlightHeight;
    bool hasReachedTargetHeight;
    const float FLIGHT_VERTICAL_SPEED = 8.0f;  // Speed for W/S controls

    // Textures for different states
    Texture standingTexture;
    Texture runningTexture;
    Texture flyingTexture;
    Texture ballTexture;
    Texture currentTexture;
    bool facingRight;
    bool isBall;


    bool loadTexture(Texture& texture, const string& filename) {
        if (!texture.loadFromFile(filename)) {
            return false;
        }
        return true;
    }

    void updateSprite() {
        Texture* newTexture = nullptr;

        // Check for flying first - highest priority when in flight mode
        if (isFlying) {
            newTexture = &flyingTexture;
            isBall = false;
        }
        // Check for max speed (ball form)
        else if (abs(velocityX) >= max_speed) {
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
        }
        else {
            sprite.setScale(-scale_x, scale_y);
            sprite.setOrigin(40, 0);
        }

        // Update main character direction for other characters
        updateMainCharacterDirection(facingRight);
    }

public:
    Tails(float start_x, float start_y, HealthManager* healthMgr, float scale = 2.5f) : Player(start_x, start_y, healthMgr, scale)
    {
        // Load textures
        loadTexture(standingTexture, "Data/tails_standing.png");
        loadTexture(runningTexture, "Data/tails_running.png");
        loadTexture(flyingTexture, "Data/tails_flying.png");
        loadTexture(ballTexture, "Data/tails_ball.png");
        
        // Set initial texture
        currentTexture = standingTexture;
        sprite.setTexture(currentTexture);
        sprite.setScale(scale_x, scale_y);
        sprite.setOrigin(0, 0);
        
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
        flightTimeRemaining = FLIGHT_MAX_TIME;
        isFlying = false;
        initialFlightHeight = 0.0f;
        hasReachedTargetHeight = false;
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
        
        // Flight controls
        if (isFlying) {
            if (Keyboard::isKeyPressed(Keyboard::W)) {
                velocityY = -FLIGHT_VERTICAL_SPEED;  // Move up
            }
            else if (Keyboard::isKeyPressed(Keyboard::S)) {
                velocityY = FLIGHT_VERTICAL_SPEED;   // Move down
            }
            else {
                velocityY = 0;  // No vertical movement when neither key is pressed
            }
        }
        
        if (Keyboard::isKeyPressed(Keyboard::LControl))
        {
            activateAbility(level);
        }
    }

    void updatePhysics(Level* level) override
    {
        if (!isFlying) {
            Player::updatePhysics(level);
        }
        else {
            // Custom flight physics
            float offset_y = player_y;
            offset_y += velocityY;

            // Check for ground collision
            int cell_size = static_cast<int>(level->getCellSize());
            int bottom_row = static_cast<int>((offset_y + hit_box_factor_y + Pheight) / cell_size);
            int left_col = static_cast<int>((player_x + hit_box_factor_x) / cell_size);
            int mid_col = static_cast<int>((player_x + hit_box_factor_x + Pwidth / 2) / cell_size);
            int right_col = static_cast<int>((player_x + hit_box_factor_x + Pwidth) / cell_size);

            // Ensure we're within bounds
            if (bottom_row >= 0 && bottom_row < level->getHeight() &&
                left_col >= 0 && left_col < level->getWidth() &&
                mid_col >= 0 && mid_col < level->getWidth() &&
                right_col >= 0 && right_col < level->getWidth()) {
                
                char bottom_left_down = level->getLevelData()[bottom_row][left_col];
                char bottom_mid_down = level->getLevelData()[bottom_row][mid_col];
                char bottom_right_down = level->getLevelData()[bottom_row][right_col];

            if (bottom_left_down == 'w' || bottom_mid_down == 'w' || bottom_right_down == 'w') {
                onGround = true;
                endFlight();
            }
            else {
                player_y = offset_y;
                onGround = false;
                }
            }

            // Apply horizontal movement
            player_x += velocityX;
        }
        updateSprite();
    }

    void activateAbility(Level* level) override {
        if (!isFlying && abilityCooldown <= 0 && flightTimeRemaining > 0) {
            isFlying = true;
            initialFlightHeight = player_y;
            hasReachedTargetHeight = false;
            velocityY = FLIGHT_LIFT;  // Strong initial lift
            abilityActive = true;
            onGround = false;  // Ensure we're not grounded when starting flight
        }
    }

    void updateAbility(float deltaTime) override {
        if (isFlying) {
            flightTimeRemaining -= deltaTime;

            // End flight if conditions are met
            if (flightTimeRemaining <= 0 || onGround) {
                endFlight();
            }
        }

        // Recharge flight time when on ground
        if (onGround && !isFlying) {
            flightTimeRemaining = min(FLIGHT_MAX_TIME, 
                flightTimeRemaining + (deltaTime * FLIGHT_RECHARGE_RATE));
        }

        // Update cooldown
        abilityCooldown = max(0.0f, abilityCooldown - deltaTime);
    }

    void endFlight() {
        isFlying = false;
        abilityActive = false;
        abilityCooldown = 5.0f;
        hasReachedTargetHeight = false;
        // Allow natural descent
        velocityY = 0;
    }

    float getMaxSpeed() override { return 10.0f; }
    float getJumpStrength() const override { return -20.0f; }
};

#endif // TAILS_H 