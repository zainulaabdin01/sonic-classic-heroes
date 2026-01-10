#pragma once
#include "Player.h"
#include "Sonic.h"
#include "Tails.h"
#include "Knuckles.h"
#include "Level.h"
#include "HealthManager.h"
#include <SFML/Graphics.hpp>

class PlayerManager
{
private:
	Player* characters[3];
	Player* currentPlayer;
	const float gap = 50.0f;
	Clock abilityClock;
	bool currentFacingRight;  
	const float PIT_THRESHOLD = 800.0f; 
	
	// Respawn system - per character tracking
	Clock respawnTimers[3];
	bool needsRespawn[3];
	float lastSafeX[3];  // Track last safe X position for each character
	const float RESPAWN_DELAY = 0.5f; 
	const int RESPAWN_BLOCKS_BEHIND = 2;  // How many blocks behind the pit to respawn

	// Starting positions
	const float START_X = 100.0f;
	const float START_Y = 100.0f;
	HealthManager* healthManager;
	
	// Find safe respawn position on solid ground
	void findSafeRespawnPosition(Level* level, float pitX, float& outX, float& outY) {
		float cellSize = level->getCellSize();
		char** levelData = level->getLevelData();
		int levelWidth = level->getWidth();
		int levelHeight = level->getHeight();
		
		// Start searching from the pit position, going backwards
		int startGridX = static_cast<int>(pitX / cellSize) - RESPAWN_BLOCKS_BEHIND;
		if (startGridX < 0) startGridX = 0;
		
		// Search for solid ground (wall 'w' or platform 'p') with empty space above
		for (int searchX = startGridX; searchX >= 0; searchX--) {
			for (int searchY = levelHeight - 2; searchY >= 0; searchY--) {
				char cell = levelData[searchY][searchX];
				char cellAbove = (searchY > 0) ? levelData[searchY - 1][searchX] : 's';
				
				// Found solid ground with space above
				if ((cell == 'w' || cell == 'p') && (cellAbove == 's' || cellAbove == 'r')) {
					outX = searchX * cellSize + cellSize / 2;
					outY = (searchY - 1) * cellSize;  // Position above the ground
					return;
				}
			}
		}
		
		// Fallback to start position if no safe spot found
		outX = START_X;
		outY = START_Y;
	}

public:
	// Constructor
	PlayerManager(HealthManager* healthMgr) : healthManager(healthMgr) {
		characters[0] = new Sonic(START_X, START_Y, healthManager);
		characters[1] = new Tails(START_X, START_Y, healthManager);
		characters[2] = new Knuckles(START_X, START_Y, healthManager);
		currentPlayer = characters[0]; // Start with Sonic
		
		// Set initial current character flags
		characters[0]->setCurrentCharacter(true);  // Sonic is current
		characters[1]->setCurrentCharacter(false); // Tails is follower
		characters[2]->setCurrentCharacter(false); // Knuckles is follower
		
		// Initialize respawn tracking
		for (int i = 0; i < 3; ++i) {
			needsRespawn[i] = false;
			lastSafeX[i] = START_X;
		}
		
		abilityClock.restart();
		currentFacingRight = true;  
	}

	~PlayerManager()
	{
		for (int i = 0; i < 3; ++i)
		{
			delete characters[i];
		}
	}

	// Reset all players to the same starting position
	void resetAllPlayers(float startX, float startY) {
		for (int i = 0; i < 3; ++i) {
			characters[i]->setPosition(startX + (i * gap), startY);
			characters[i]->setVelocity(0, 0);
			characters[i]->setOnGround(false);
			characters[i]->setVisible(true);
			needsRespawn[i] = false;
		}
	}

	//Switch character
	void switchCharacter() 
	{
		// Check cooldown BEFORE switching to prevent rapid switching
		static Clock switchCooldown;
		if (switchCooldown.getElapsedTime().asSeconds() < 0.5f) {
			return;
		}
		switchCooldown.restart();

		float x = currentPlayer->getX();
		float y = currentPlayer->getY();
		float vx = currentPlayer->getVelX();
		float vy = currentPlayer->getVelY();

		int currentIndex = 0;
		for (int i = 0; i < 3; ++i) {
			if (currentPlayer == characters[i]) {
				currentIndex = i;
				break;
			}
		}

		for (int i = 0; i < 3; ++i) {
			characters[i]->setCurrentCharacter(false);
		}

		// Switch to next character
		currentIndex = (currentIndex + 1) % 3;
		currentPlayer = characters[currentIndex];

		currentPlayer->setCurrentCharacter(true);

		// Transfer position and velocity
		currentPlayer->setPosition(x, y);
		currentPlayer->setVelocity(vx, vy);

		Player::updateMainCharacterDirection(currentFacingRight);
	}

	void handleInput(Level* level)
	{
		static Clock switchCooldown;

		currentPlayer->handleInput(level);

		// Update facing direction based on current character's velocity
		if (currentPlayer->getVelX() > 0) {
			currentFacingRight = true;
			Player::updateMainCharacterDirection(true);
		}
		else if (currentPlayer->getVelX() < 0) {
			currentFacingRight = false;
			Player::updateMainCharacterDirection(false);
		}

		if (Keyboard::isKeyPressed(Keyboard::Z) &&
			switchCooldown.getElapsedTime().asSeconds() > 0.5f) {
			switchCharacter();
			switchCooldown.restart();
		}

		if (Keyboard::isKeyPressed(Keyboard::LControl)) {
			currentPlayer->activateAbility(level);
		}
	}

	void updatePhysics(Level* level)
	{
		float deltaTime = abilityClock.restart().asSeconds();
		
		bool jumpCommand = currentPlayer->hasJustJumped();
		currentPlayer->updatePhysics(level);
		currentPlayer->updateAbility(deltaTime);
		currentPlayer->resetJumpFlag();

		// Track last safe position for current player (when on ground)
		int currentIdx = 0;
		for (int i = 0; i < 3; ++i) {
			if (characters[i] == currentPlayer) {
				currentIdx = i;
				break;
			}
		}
		if (currentPlayer->isOnGround() && currentPlayer->getY() < PIT_THRESHOLD) {
			lastSafeX[currentIdx] = currentPlayer->getX();
		}

		// Check if current player fell into pit - respawn on nearest solid ground
		if (currentPlayer->getY() > PIT_THRESHOLD) {
			float respawnX, respawnY;
			findSafeRespawnPosition(level, lastSafeX[currentIdx], respawnX, respawnY);
			
			// Respawn all players at the safe position
			for (int i = 0; i < 3; ++i) {
				characters[i]->setPosition(respawnX + (i * gap), respawnY);
				characters[i]->setVelocity(0, 0);
				characters[i]->setOnGround(false);
				characters[i]->setVisible(true);
				needsRespawn[i] = false;
				lastSafeX[i] = respawnX;
			}
			return;
		}

		float mainX = currentPlayer->getX();
		float mainY = currentPlayer->getY();

		// Calculate follower positions (1 = first follower, 2 = second follower)
		int followerSlot = 1;

		// Update followers
		for (int i = 0; i < 3; ++i) {
			if (characters[i] == currentPlayer) {
				continue;  // Skip the current player
			}

			// Check if this follower fell into pit - immediately respawn behind active player
			if (characters[i]->getY() > PIT_THRESHOLD) {
				// Instantly respawn behind the current player
				float respawnX = mainX - (gap * followerSlot);
				characters[i]->setPosition(respawnX, mainY);
				characters[i]->setVelocity(0, 0);
				characters[i]->setOnGround(false);
				characters[i]->setVisible(true);
				
				followerSlot++;
				continue;
			}

			// Check if follower is too far behind (off-screen) - teleport to player
			float distanceBehind = mainX - characters[i]->getX();
			if (distanceBehind > 600.0f) {  // More than half screen behind
				// Instantly teleport behind the current player
				float respawnX = mainX - (gap * followerSlot);
				characters[i]->setPosition(respawnX, mainY);
				characters[i]->setVelocity(currentPlayer->getVelX(), 0);
				characters[i]->setOnGround(false);
				
				followerSlot++;
				continue;
			}

			// Normal following behavior
			float targetX = mainX - (gap * followerSlot);
			float dx = targetX - characters[i]->getX();

			// Smooth following with proportional speed
			float followSpeed = min(characters[i]->getMaxSpeed() * 0.8f, abs(dx) * 0.2f);
			if (dx > 5.0f) {
				characters[i]->setVelX(followSpeed);
			}
			else if (dx < -5.0f) {
				characters[i]->setVelX(-followSpeed);
			}
			else {
				characters[i]->setVelX(dx * 0.5f);
			}

			// Synchronized jumping
			if (jumpCommand && characters[i]->isOnGround()) {
				characters[i]->setVelY(characters[i]->getJumpStrength());
				characters[i]->setOnGround(false);
			}

			characters[i]->updatePhysics(level);
			characters[i]->updateAbility(deltaTime);
			
			followerSlot++;
		}
	}

	void draw(RenderWindow& window, float camera_offset_x) 
	{
		for (int i = 0; i < 3; ++i) 
		{
			characters[i]->draw(window, camera_offset_x);
		}
	}

	Player* getCurrentPlayer() const { return currentPlayer; }
};