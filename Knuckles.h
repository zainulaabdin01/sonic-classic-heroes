#ifndef KNUCKLES_H
#define KNUCKLES_H

#include "Player.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

using namespace sf;
using namespace std;

class Knuckles : public Player
{
private:
    bool isPunching;
    const float PUNCH_DURATION = 0.3f;
    const int PUNCH_RANGE = 1;

    // Textures for different states
    Texture standingTexture;
    Texture runningTexture;
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
        }
        else {
            sprite.setScale(-scale_x, scale_y);
            sprite.setOrigin(40, 0);
        }

        // Update main character direction for other characters
        updateMainCharacterDirection(facingRight);
    }

public:
    Knuckles(float start_x, float start_y, HealthManager* healthMgr, float scale = 2.5f) : Player(start_x, start_y, healthMgr, scale)
    {
        // Load textures
        loadTexture(standingTexture, "Data/knuckles_standing.png");
        loadTexture(runningTexture, "Data/knuckles_running.png");
        loadTexture(ballTexture, "Data/knuckles_ball.png");
        
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
        isPunching = false;
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

    void updateAbility(float deltaTime) override {
        if (isPunching) {
            // End punch after duration
            if (deltaTime >= PUNCH_DURATION) {
                isPunching = false;
                abilityActive = false;
            }
        }
    }

    void activateAbility(Level* level) override {
        // Remove the isPunching check since we want to allow continuous punching
        isPunching = true;
        abilityActive = true;
        breakWalls(level);
    }

    void breakWalls(Level* level) {
        int cell_size = level->getCellSize();
        char** lvl = level->getLevelData();
        Obstacle** obstacles = level->getObstacles();
        int obstacleCount = level->getObstacleCount();
        // Check adjacent cells in a square pattern
        int centerX = static_cast<int>(player_x + Pwidth / 2) / cell_size;
        int centerY = static_cast<int>(player_y + Pheight / 2) / cell_size;

        for (int dx = -PUNCH_RANGE; dx <= PUNCH_RANGE; dx++) {
            for (int dy = -PUNCH_RANGE; dy <= PUNCH_RANGE; dy++) {
                int x = centerX + dx;
                int y = centerY + dy;
                // Check if within level bounds
                if (x >= 0 && y >= 0 && x < level->getWidth() && y < level->getHeight()) {
                    // Break both breakable walls ('b') 
                    if (lvl[y][x] == 'b') {
                        // Check for breakable wall objects
                        for (int i = 0; i < obstacleCount; ++i) {
                            BreakableWall* breakableWall = static_cast<BreakableWall*>(obstacles[i]);
                            float wallX = breakableWall->getX();
                            float wallY = breakableWall->getY();
                            // Check if this is the wall at the current grid position
                            if (static_cast<int>(wallX / cell_size) == x && 
                                static_cast<int>(wallY / cell_size) == y) {
                                // Break the wall with Knuckles' punch
                                breakableWall->takeDamage(0, true);
                                break;
                            }
                        }
                        // Convert to empty space in the level grid
                        lvl[y][x] = 's';
                    }
                }
            }
        }
    }

    float getMaxSpeed() override { return 12.0f; }
    float getJumpStrength() const override { return -22.0f; }
};

#endif // KNUCKLES_H 