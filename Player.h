#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include "Level.h"
#include "Obstacle.h"
#include "BreakableWall.h"
#include "HealthManager.h"

using namespace sf;
using namespace std;


// Player class declaration
class Player {
protected:
    float player_x, player_y;
    float velocityX, velocityY;
    Texture texture;
    Sprite sprite;
    float scale_x, scale_y;
    int Pheight, Pwidth;
    int hit_box_factor_x, hit_box_factor_y;
    bool onGround;
    bool justJumped;
    bool isVisible;  
    bool shouldTransitionLevel; 
    bool isInvulnerable;
    Clock invulnClock;
    const float INVULN_TIME = 1.0f;
    bool isCurrentCharacter;  

    HealthManager* healthManager;
    static bool isGameOver;
    static bool mainCharacterFacingRight;

    // Physics constants - tuned for smooth, realistic movement
    float max_speed = 15;
    const float jumpStrength = -18.0f;
    const float gravity = 0.65f;
    const float terminal_Velocity = 18;
    
    // Acceleration/deceleration - different values for smooth feel
    const float groundAcceleration = 0.45f;    // How fast we speed up on ground
    const float airAcceleration = 0.25f;       // Reduced control in air
    const float groundFriction = 0.92f;        // Multiplier when not pressing keys (1.0 = no friction)
    const float airFriction = 0.98f;           // Less friction in air
    const float turnAroundBoost = 0.6f;        // Extra decel when changing direction
    const float skidThreshold = 8.0f;          // Speed at which skidding starts
    
    // Jump physics
    const float jumpCutMultiplier = 0.4f;      // Multiplier when releasing jump early
    const float coyoteTime = 0.1f;             // Time after leaving ground you can still jump
    const float jumpBufferTime = 0.1f;         // Time before landing that jump input is remembered
    float coyoteTimer = 0.0f;
    float jumpBufferTimer = 0.0f;
    bool jumpHeld = false;
    bool jumpReleased = true;                  // Prevents auto-jumping when holding space
    
    // Smooth speed ramping
    float targetVelocityX = 0.0f;
    const float velocitySmoothing = 0.15f;     // Lerp factor for velocity

    float abilityCooldown;
    float abilityDuration;
    bool abilityActive;
    Clock abilityTimer;
    Clock frameTimer;

    Music jumpMusic;

    bool check_wall_collision(char** lvl, float x, float y, int cell_size, int width, int height) {
        int gridX = static_cast<int>(x) / cell_size;
        int gridY = static_cast<int>(y) / cell_size;

        if (gridX < 0 || gridX >= width || gridY < 0 || gridY >= height)
            return false;

        return lvl[gridY][gridX] == 'w' || lvl[gridY][gridX] == 'b';  // Check for both walls and breakable walls
    }

    bool check_platform_collision(char** lvl, float x, float bottom_y, int cell_size, int width, int height) {
        int gridX = static_cast<int>(x) / cell_size;
        int gridY = static_cast<int>(bottom_y) / cell_size;

        if (gridX < 0 || gridX >= width || gridY < 0 || gridY >= height)
            return false;

        return lvl[gridY][gridX] == 'p';  // Check for platforms
    }

    void handleCollisions(Level* level) {
        int cell_size = level->getCellSize();
        char** lvl = level->getLevelData();
        int width = level->getWidth();
        int height = level->getHeight();

        // Store original position
        float original_x = player_x;
        float original_y = player_y;

        // Try horizontal movement
        player_x += velocityX;

        // Check horizontal collisions
        bool collisionLeftWall = check_wall_collision(lvl, player_x + hit_box_factor_x, player_y + hit_box_factor_y, cell_size, width, height) ||
                               check_wall_collision(lvl, player_x + hit_box_factor_x, player_y + Pheight - hit_box_factor_y, cell_size, width, height);
        bool collisionRightWall = check_wall_collision(lvl, player_x + Pwidth - hit_box_factor_x, player_y + hit_box_factor_y, cell_size, width, height) ||
                                check_wall_collision(lvl, player_x + Pwidth - hit_box_factor_x, player_y + Pheight - hit_box_factor_y, cell_size, width, height);

        // If there's a horizontal collision, revert the movement
        if (collisionLeftWall || collisionRightWall) {
            player_x = original_x;
            velocityX = 0;
        }

        // Try vertical movement
        float offset_y = player_y + velocityY;

        // Check if player is in the last pit
        if (level->isInLastPit(player_x, player_y)) {
            shouldTransitionLevel = true;
            return;
        }

        // Wall collisions (full collision)
        bool collisionBottomWallLeft = check_wall_collision(lvl, player_x + hit_box_factor_x,
            offset_y + Pheight, cell_size, width, height);
        bool collisionBottomWallRight = check_wall_collision(lvl, player_x + Pwidth - hit_box_factor_x,
            offset_y + Pheight, cell_size, width, height);
        bool collisionBottomWallMid = check_wall_collision(lvl, player_x + Pwidth / 2,
            offset_y + Pheight, cell_size, width, height);

        // Platform collisions (top-side only)
        bool collisionPlatformLeft = check_platform_collision(lvl, player_x + hit_box_factor_x,
            offset_y + Pheight, cell_size, width, height);
        bool collisionPlatformRight = check_platform_collision(lvl, player_x + Pwidth - hit_box_factor_x,
            offset_y + Pheight, cell_size, width, height);
        bool collisionPlatformMid = check_platform_collision(lvl, player_x + Pwidth / 2,
            offset_y + Pheight, cell_size, width, height);

        // Handle wall collisions (full collision)
        if (collisionBottomWallLeft || collisionBottomWallMid || collisionBottomWallRight) {
            player_y = ((int)(offset_y + Pheight) / cell_size) * cell_size - Pheight - 1;
            onGround = true;
            velocityY = 0;
        }
        // Handle platform collisions (only when falling down)
        else if ((collisionPlatformLeft || collisionPlatformMid || collisionPlatformRight) && velocityY > 0) {
            player_y = ((int)(offset_y + Pheight) / cell_size) * cell_size - Pheight - 1;
            onGround = true;
            velocityY = 0;
        }
        else {
            player_y = offset_y;
            onGround = false;
        }
    }

public:
    Player(float start_x, float start_y, HealthManager* healthMgr, float scale = 2.5f) :
        player_x(start_x), player_y(start_y), scale_x(scale), scale_y(scale),
        isInvulnerable(false), isCurrentCharacter(false), healthManager(healthMgr)
    {
        // Initialize state
        velocityX = 0;
        velocityY = 0;
        onGround = false;
        isVisible = true;  // Initialize as visible
        shouldTransitionLevel = false;
        jumpMusic.openFromFile("Data/Jump.wav");
        jumpMusic.setVolume(30);
    }

    // Static method to check if game is over
    static bool isGameOverState() { return isGameOver; }

    // Method to set current character status
    void setCurrentCharacter(bool isCurrent) {
        isCurrentCharacter = isCurrent;
        if (isCurrent) {
            cout << "Current character health: " << (healthManager ? healthManager->getHealth() : 0) << " hearts" << endl;
        }
    }

    // Static method to update main character direction
    static void updateMainCharacterDirection(bool facingRight) {
        mainCharacterFacingRight = facingRight;
    }

    virtual void handleInput(Level* level)
    {
        float deltaTime = frameTimer.restart().asSeconds();
        if (deltaTime > 0.1f) deltaTime = 0.016f; // Cap delta time
        
        PhysicsConfig* phys = level ? level->getPhysicsConfig() : nullptr;
        float maxSpd = phys ? phys->getMaxSpeed() : max_speed;
        
        // Choose acceleration based on ground/air state
        float currentAccel = onGround ? groundAcceleration : airAcceleration;
        float currentFriction = onGround ? groundFriction : airFriction;
        
        bool movingRight = Keyboard::isKeyPressed(Keyboard::Right);
        bool movingLeft = Keyboard::isKeyPressed(Keyboard::Left);
        
        // Horizontal movement with smooth acceleration
        if (movingRight && !movingLeft) {
            // Check if turning around (was moving left, now moving right)
            if (velocityX < -skidThreshold) {
                // Skidding - apply extra deceleration for snappy turn-around
                velocityX += currentAccel + turnAroundBoost;
            } else {
                // Normal acceleration with diminishing returns near max speed
                float speedRatio = abs(velocityX) / maxSpd;
                float adjustedAccel = currentAccel * (1.0f - speedRatio * 0.5f);
                velocityX += adjustedAccel;
            }
            if (velocityX > maxSpd) velocityX = maxSpd;
        }
        else if (movingLeft && !movingRight) {
            // Check if turning around (was moving right, now moving left)
            if (velocityX > skidThreshold) {
                // Skidding - apply extra deceleration for snappy turn-around
                velocityX -= currentAccel + turnAroundBoost;
            } else {
                // Normal acceleration with diminishing returns near max speed
                float speedRatio = abs(velocityX) / maxSpd;
                float adjustedAccel = currentAccel * (1.0f - speedRatio * 0.5f);
                velocityX -= adjustedAccel;
            }
            if (velocityX < -maxSpd) velocityX = -maxSpd;
        }
        else {
            // Apply friction when not pressing left/right (multiplicative for smooth stop)
            velocityX *= currentFriction;
            // Snap to zero when very slow to prevent sliding forever
            if (abs(velocityX) < 0.1f) velocityX = 0;
        }

        // Update coyote time (grace period after leaving ground)
        if (onGround) {
            coyoteTimer = coyoteTime;
        } else {
            coyoteTimer -= deltaTime;
        }
        
        // Jump buffer - remember jump input slightly before landing
        if (Keyboard::isKeyPressed(Keyboard::Space)) {
            if (jumpReleased) {
                jumpBufferTimer = jumpBufferTime;
                jumpReleased = false;
            }
            jumpHeld = true;
        } else {
            jumpHeld = false;
            jumpReleased = true;
        }
        jumpBufferTimer -= deltaTime;
        
        // Attempt jump if buffer is active and we can jump
        if (jumpBufferTimer > 0 && (onGround || coyoteTimer > 0)) {
            performJump();
            jumpBufferTimer = 0;
            coyoteTimer = 0;
        }
        
        // Variable jump height - cut velocity when releasing jump button early
        if (!jumpHeld && velocityY < 0 && !onGround) {
            velocityY *= jumpCutMultiplier + (1.0f - jumpCutMultiplier) * 0.5f;
        }
    }

    void jump() {
        // Public jump method for external calls (followers, etc.)
        if (onGround || coyoteTimer > 0) {
            performJump();
        }
    }
    
    void performJump() {
        velocityY = jumpStrength;
        onGround = false;
        justJumped = true;
        coyoteTimer = 0;
        jumpMusic.play();
    }

    virtual void updatePhysics(Level* level) 
    {
        PhysicsConfig* phys = level ? level->getPhysicsConfig() : nullptr;
        float grav = phys ? phys->getGravity() : gravity;
        float termVel = phys ? phys->getTerminalVelocity() : terminal_Velocity;
        
        // Apply gravity with smooth acceleration (not instant)
        if (!onGround) {
            // Apply stronger gravity when falling (more satisfying arc)
            float gravityMultiplier = (velocityY > 0) ? 1.2f : 1.0f;
            velocityY += grav * gravityMultiplier;
            
            // Smooth approach to terminal velocity
            if (velocityY > termVel) {
                velocityY = termVel;
            }
        } else {
            // Reset vertical velocity when grounded
            if (velocityY > 0) {
                velocityY = 0;
            }
        }

        // Handle collisions
        handleCollisions(level);

        // Check obstacle collisions
        float damage = level->checkObstacleCollisions(player_x, player_y, Pwidth, Pheight);
        if (damage > 0) {
            takeDamage();
        }

        // Check collectible (ring) collisions
        level->checkCollectibleCollisions(player_x, player_y, Pwidth, Pheight);

        // Reset color and invulnerability after duration
        if (isInvulnerable && invulnClock.getElapsedTime().asSeconds() >= INVULN_TIME) {
            isInvulnerable = false;
            sprite.setColor(Color::White);
        }

        // Clamp horizontal speed
        float maxSpd = phys ? phys->getMaxSpeed() : max_speed;
        if (velocityX > maxSpd) {
            velocityX = maxSpd;
        } else if (velocityX < -maxSpd) {
            velocityX = -maxSpd;
        }
    }

    virtual void draw(RenderWindow& window, float camera_offset_x) {
        if (isVisible) {  // Only draw if visible
        sprite.setPosition((player_x - camera_offset_x), player_y);
        window.draw(sprite);
        }
    }

    virtual float getMaxSpeed() = 0;
	virtual void setPosition(float x, float y) 
    { 
        player_x = x; 
        player_y = y; 
    }
    virtual void setVelocity(float vx, float vy) 
    { 
        velocityX = vx; 
        velocityY = vy; 
    }
    virtual float getJumpStrength() const = 0;
    virtual void activateAbility(Level* level) = 0;
    virtual void updateAbility(float deltaTime) = 0;
    virtual bool isAbilityActive() const { return abilityActive; }

    float getX() const { return player_x; }
    float getY() const { return player_y; }
    int getWidth() const { return Pwidth; }
	int getHeight() const { return Pheight; }
	float getVelX() const { return velocityX; }
	float getVelY() const { return velocityY; }
	void setVelX(float vx) { velocityX = vx; }
	void setVelY(float vy) { velocityY = vy; }
    bool hasJustJumped() const { return justJumped; }
    void resetJumpFlag() { justJumped = false; }
	bool isOnGround() const { return onGround; }
	void setOnGround(bool ground) { onGround = ground; }
    void setVisible(bool visible) { isVisible = visible; }
    bool getVisible() const { return isVisible; }

    // check if level transition is needed
    bool needsLevelTransition() const {
        return shouldTransitionLevel;
    }

    //reset level transition flag
    void resetLevelTransition() {
        shouldTransitionLevel = false;
    }

    // Method to handle damage (always decrements health by 1, applies invulnerability)
    void takeDamage() {
        if (isCurrentCharacter && !isInvulnerable) {
            // Main player takes damage
            if (healthManager) healthManager->decrementHealth();
            cout << "Health: " << (healthManager ? healthManager->getHealth() : 0) << endl;
            // Visual feedback
            sprite.setColor(Color(255, 0, 0, 128));
            // Set invulnerability
            isInvulnerable = true;
            invulnClock.restart();
            if (healthManager && healthManager->getHealth() <= 0) {
                cout << "Game Over! Health reached 0" << endl;
                isGameOver = true;
            }
        }
        else if (!isCurrentCharacter) {
            // Follower only shows visual feedback
            sprite.setColor(Color(255, 0, 0, 128));
        }
    }

    int getHealth() const { return healthManager ? healthManager->getHealth() : 0; }
    bool isAlive() const { return healthManager ? healthManager->getHealth() > 0 : false; }

    void incrementSharedHealth() { if (healthManager) healthManager->incrementHealth(); }

    bool getIsInvulnerable() const { return isInvulnerable; }
	void setIsInvulnerable(bool invulnerable) { isInvulnerable = invulnerable; }

    void updateInvulnerability() {
        if (isInvulnerable && invulnClock.getElapsedTime().asSeconds() > INVULN_TIME) {
            isInvulnerable = false;
        }
    }
};

bool Player::isGameOver = false;
bool Player::mainCharacterFacingRight = true;
#endif // PLAYER_H