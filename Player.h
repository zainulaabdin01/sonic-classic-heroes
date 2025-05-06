#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

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

    // Static direction tracker for all characters
    static bool mainCharacterFacingRight;

    // Constants
    float max_speed = 15;
    const float jumpStrength = -20;
    const float gravity = 0.98f;
    const float terminal_Velocity = 20;
    const float acceleration = 0.2f;

    float abilityCooldown;
    float abilityDuration;
    bool abilityActive;
    Clock abilityTimer;

public:
    Player(float start_x, float start_y, float scale = 2.5f) :
        player_x(start_x), player_y(start_y), scale_x(scale), scale_y(scale) 
    {
        // Initialize state
        velocityX = 0;
        velocityY = 0;
        onGround = false;
    }

    // Static method to update main character direction
    static void updateMainCharacterDirection(bool facingRight) {
        mainCharacterFacingRight = facingRight;
    }

    virtual void handleInput() 
    {
        // Horizontal movement
        if (Keyboard::isKeyPressed(Keyboard::Right)) {
            velocityX += acceleration;
            if (velocityX > max_speed) velocityX = max_speed;
        }
        else if (Keyboard::isKeyPressed(Keyboard::Left)) {
            velocityX -= acceleration;
            if (velocityX < -max_speed) velocityX = -max_speed;
        }
        else 
        {
            velocityX = 0; // Maintain original behavior
        }

		if (Keyboard::isKeyPressed(Keyboard::Space)) 
        {
			jump();
		}
    }

    void jump() {
        if (onGround) {
            velocityY = jumpStrength;
            onGround = false;
			justJumped = true;
        }
    }

    virtual void updatePhysics(char** lvl, const int cell_size) 
    {
        // Gravity and vertical collisions
        float offset_y = player_y;
        offset_y += velocityY;

        char bottom_left_down = lvl[(int)(offset_y + hit_box_factor_y + Pheight) / cell_size][(int)(player_x + hit_box_factor_x) / cell_size];
        char bottom_mid_down = lvl[(int)(offset_y + hit_box_factor_y + Pheight) / cell_size][(int)(player_x + hit_box_factor_x + Pwidth / 2) / cell_size];
        char bottom_right_down = lvl[(int)(offset_y + hit_box_factor_y + Pheight) / cell_size][(int)(player_x + hit_box_factor_x + Pwidth) / cell_size];

        if (bottom_left_down == 'w' || bottom_mid_down == 'w' || bottom_right_down == 'w') {
            onGround = true;
        }
        else {
            player_y = offset_y;
            onGround = false;
        }

        if (!onGround) {
            velocityY += gravity;
            if (velocityY >= terminal_Velocity) velocityY = terminal_Velocity;
        }
        else {
            velocityY = 0;
        }

        // Apply horizontal movement
        player_x += velocityX;
    }

    virtual void draw(RenderWindow& window, float camera_offset_x) {
        sprite.setPosition((player_x - camera_offset_x), player_y);
        window.draw(sprite);
    }

	//virtual void useSpecialAbility() = 0;
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
    virtual void activateAbility(char** lvl, int cell_size) = 0;
    virtual void updateAbility(float deltaTime) = 0;
    virtual bool isAbilityActive() const { return abilityActive; }

    float getX() const { return player_x; }
    float getY() const { return player_y; }
	float getVelX() const { return velocityX; }
	float getVelY() const { return velocityY; }
	void setVelX(float vx) { velocityX = vx; }
	void setVelY(float vy) { velocityY = vy; }
    bool hasJustJumped() const { return justJumped; }
    void resetJumpFlag() { justJumped = false; }
	bool isOnGround() const { return onGround; }
	void setOnGround(bool ground) { onGround = ground; }
};

// Initialize static member
bool Player::mainCharacterFacingRight = true;

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
            sprite.setOrigin(sprite.getLocalBounds().width, 0);
        }

        // Update main character direction for other characters
        updateMainCharacterDirection(facingRight);
    }

public:
    Sonic(float start_x, float start_y, float scale = 2.5f) : Player(start_x, start_y, scale) 
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

    void handleInput() override
    {
        Player::handleInput();
        
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
            activateAbility(nullptr, 0);
        }
    }

    void updatePhysics(char** lvl, const int cell_size) override
    {
        Player::updatePhysics(lvl, cell_size);
        updateSprite();
    }

    void activateAbility(char** lvl, int cell_size) override
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

    // Textures for different states
    Texture standingTexture;
    Texture runningTexture;
    Texture flyingTexture;
    Texture ballTexture;
    Texture currentTexture;
    bool facingRight;
    bool isBall;

    enum class TextureState {
        STANDING,
        RUNNING,
        FLYING,
        BALL
    };
    TextureState currentState;

    bool loadTexture(Texture& texture, const string& filename) {
        if (!texture.loadFromFile(filename)) {
            return false;
        }
        return true;
    }

    void updateSprite() {
        Texture* newTexture = nullptr;
        TextureState newState = currentState;
        
        // Determine new state based on conditions
        if (abs(velocityX) >= max_speed) {
            newState = TextureState::BALL;
        }
        else if (isFlying) {
            newState = TextureState::FLYING;
        }
        else if (abs(velocityX) > 0) {
            newState = TextureState::RUNNING;
        }
        else {
            newState = TextureState::STANDING;
        }

        // Only update texture if state changed
        if (newState != currentState) {
            switch (newState) {
                case TextureState::BALL:
                    newTexture = &ballTexture;
                    isBall = true;
                    break;
                case TextureState::FLYING:
                    newTexture = &flyingTexture;
                    isBall = false;
                    break;
                case TextureState::RUNNING:
                    newTexture = &runningTexture;
                    isBall = false;
                    break;
                case TextureState::STANDING:
                    newTexture = &standingTexture;
                    isBall = false;
                    break;
            }
            currentState = newState;
            currentTexture = *newTexture;
            sprite.setTexture(currentTexture);
        }

        // Use main character's direction
        facingRight = mainCharacterFacingRight;
        
        // Set sprite direction and position
        if (facingRight) {
            sprite.setScale(scale_x, scale_y);
            sprite.setOrigin(0, 0);
        } else {
            sprite.setScale(-scale_x, scale_y);
            sprite.setOrigin(sprite.getLocalBounds().width, 0);
        }
    }

public:
    Tails(float start_x, float start_y, float scale = 2.5f) : Player(start_x, start_y, scale)
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
        currentState = TextureState::STANDING;
    }

    void handleInput() override
    {
        Player::handleInput();
        
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
            activateAbility(nullptr, 0);
        }
    }

    void updatePhysics(char** lvl, const int cell_size) override
    {
        if (!isFlying) {
            Player::updatePhysics(lvl, cell_size);
        }
        else {
            // Custom flight physics
            float offset_y = player_y;
            offset_y += velocityY;

            // Check for ground collision
            char bottom_left_down = lvl[(int)(offset_y + hit_box_factor_y + Pheight) / cell_size][(int)(player_x + hit_box_factor_x) / cell_size];
            char bottom_mid_down = lvl[(int)(offset_y + hit_box_factor_y + Pheight) / cell_size][(int)(player_x + hit_box_factor_x + Pwidth / 2) / cell_size];
            char bottom_right_down = lvl[(int)(offset_y + hit_box_factor_y + Pheight) / cell_size][(int)(player_x + hit_box_factor_x + Pwidth) / cell_size];

            if (bottom_left_down == 'w' || bottom_mid_down == 'w' || bottom_right_down == 'w') {
                onGround = true;
                endFlight();
            }
            else {
                player_y = offset_y;
                onGround = false;
            }

            // Apply horizontal movement
            player_x += velocityX;
        }
        updateSprite();
    }

    void activateAbility(char** lvl, int cell_size) override {
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
            
            // Initial ascent phase
            if (!hasReachedTargetHeight) {
                if (player_y <= initialFlightHeight - FLIGHT_TARGET_HEIGHT) {
                    hasReachedTargetHeight = true;
                    velocityY = 0;  // Stop ascending
                }
                else {
                    velocityY = FLIGHT_LIFT;  // Keep ascending
                }
            }
            // Maintain height phase
            else {
                if (Keyboard::isKeyPressed(Keyboard::Space)) {
                    velocityY = FLIGHT_LIFT;  // Allow ascending
                }
                else {
                    // Maintain height with slight gravity
                    velocityY = min(velocityY + gravity * 0.1f, 0.0f);
                }
            }

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

class Knuckles : public Player
{
private:
    bool isPunching;
    float punchCooldown;
    const float PUNCH_DURATION = 0.3f;
    const float PUNCH_COOLDOWN = 1.0f;
    const int PUNCH_RANGE = 1;

    // Textures for different states
    Texture standingTexture;
    Texture runningTexture;
    Texture ballTexture;
    Texture currentTexture;
    bool facingRight;
    bool isBall;

    enum class TextureState {
        STANDING,
        RUNNING,
        BALL
    };
    TextureState currentState;

    bool loadTexture(Texture& texture, const string& filename) {
        if (!texture.loadFromFile(filename)) {
            return false;
        }
        return true;
    }

    void updateSprite() {
        Texture* newTexture = nullptr;
        TextureState newState = currentState;
        
        // Determine new state based on conditions
        if (abs(velocityX) >= max_speed) {
            newState = TextureState::BALL;
        }
        else if (abs(velocityX) > 0) {
            newState = TextureState::RUNNING;
        }
        else {
            newState = TextureState::STANDING;
        }

        // Only update texture if state changed
        if (newState != currentState) {
            switch (newState) {
                case TextureState::BALL:
                    newTexture = &ballTexture;
                    isBall = true;
                    break;
                case TextureState::RUNNING:
                    newTexture = &runningTexture;
                    isBall = false;
                    break;
                case TextureState::STANDING:
                    newTexture = &standingTexture;
                    isBall = false;
                    break;
            }
            currentState = newState;
            currentTexture = *newTexture;
            sprite.setTexture(currentTexture);
        }

        // Use main character's direction
        facingRight = mainCharacterFacingRight;
        
        // Set sprite direction and position
        if (facingRight) {
            sprite.setScale(scale_x, scale_y);
            sprite.setOrigin(0, 0);
        } else {
            sprite.setScale(-scale_x, scale_y);
            sprite.setOrigin(sprite.getLocalBounds().width, 0);
        }
    }

public:
    Knuckles(float start_x, float start_y, float scale = 2.5f) : Player(start_x, start_y, scale)
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
        punchCooldown = 0;
        isPunching = false;
        facingRight = true;
        isBall = false;
        currentState = TextureState::STANDING;
    }

    void handleInput() override
    {
        Player::handleInput();
        
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
            activateAbility(nullptr, 0);
        }
    }

    void updatePhysics(char** lvl, const int cell_size) override
    {
        Player::updatePhysics(lvl, cell_size);
        updateSprite();
    }

    void activateAbility(char** lvl, int cell_size) override {
        if (punchCooldown <= 0 && !isPunching) {
            isPunching = true;
            punchCooldown = PUNCH_COOLDOWN;
            abilityActive = true;
            breakWalls(lvl, cell_size);
        }
    }

    void updateAbility(float deltaTime) override {
        if (isPunching) {
            // End punch after duration
            if (deltaTime >= PUNCH_DURATION) {
                isPunching = false;
                abilityActive = false;
            }
        }
        punchCooldown = std::max(0.0f, punchCooldown - deltaTime);
    }

    void breakWalls(char** lvl, int cell_size) {
        // Check adjacent cells in a square pattern
        int centerX = static_cast<int>(player_x + Pwidth / 2) / cell_size;
        int centerY = static_cast<int>(player_y + Pheight / 2) / cell_size;

        for (int dx = -PUNCH_RANGE; dx <= PUNCH_RANGE; dx++) {
            for (int dy = -PUNCH_RANGE; dy <= PUNCH_RANGE; dy++) {
                int x = centerX + dx;
                int y = centerY + dy;
                
                // Check if within level bounds
                if (x >= 0 && y >= 0 && x < 110 && y < 14) {
                    // Break both breakable walls ('b') and punchable walls ('p')
                    if (lvl[y][x] == 'b' || lvl[y][x] == 'p') {
                        lvl[y][x] = 's'; // Convert to empty space
                    }
                }
            }
        }
    }

    float getMaxSpeed() override { return 12.0f; }
    float getJumpStrength() const override { return -22.0f; }
};

#endif