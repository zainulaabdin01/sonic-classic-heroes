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
	
	// Respawn system
	Clock followerRespawnTimer;
	const float RESPAWN_DELAY = 0.7f; 
	bool isFollowerRespawning;

	// Starting positions
	const float START_X = 100.0f;
	const float START_Y = 100.0f;
	HealthManager* healthManager;

public:
	// Constructor
	PlayerManager(HealthManager* healthMgr) : isFollowerRespawning(false), healthManager(healthMgr) {
		characters[0] = new Sonic(START_X, START_Y, healthManager);
		characters[1] = new Tails(START_X, START_Y, healthManager);
		characters[2] = new Knuckles(START_X, START_Y, healthManager);
		currentPlayer = characters[0]; // Start with Sonic
		
		// Set initial current character flags
		characters[0]->setCurrentCharacter(true);  // Sonic is current
		characters[1]->setCurrentCharacter(false); // Tails is follower
		characters[2]->setCurrentCharacter(false); // Knuckles is follower
		
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
			characters[i]->setPosition(startX, startY);
			characters[i]->setVelocity(0, 0);
			characters[i]->setOnGround(false);
			characters[i]->setVisible(true);
		}
	}

	//Switch character
	void switchCharacter() 
	{
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

		// Add a small cooldown to prevent rapid switching
		static Clock switchCooldown;
		if (switchCooldown.getElapsedTime().asSeconds() < 0.5f) {
			return;
		}
		switchCooldown.restart();
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

		// Check if current player fell into pit
		if (currentPlayer->getY() > PIT_THRESHOLD) {
			resetAllPlayers(100, 100);
			return;
		}

		float mainX = currentPlayer->getX();
		float mainY = currentPlayer->getY();

		// Update followers
		for (int i = 0; i < 3; ++i) {
			if (characters[i] != currentPlayer) {
				if (characters[i]->getY() > PIT_THRESHOLD) {
					if (!isFollowerRespawning) {
						followerRespawnTimer.restart();
						isFollowerRespawning = true;
						characters[i]->setVisible(false);  // Make follower invisible
					}
					
					// Check if respawn delay has passed
					if (followerRespawnTimer.getElapsedTime().asSeconds() >= RESPAWN_DELAY) {
						int followerIndex = 1;
						for (int j = 0; j < 3; ++j) {
							if (j != i && characters[j] != currentPlayer) {
								followerIndex = 2;
								break;
							}
						}
						
						// Respawn next to the current player
						characters[i]->setPosition(mainX - (gap * followerIndex), mainY);
						characters[i]->setVelocity(0, 0);
						characters[i]->setOnGround(false);
						characters[i]->setVisible(true);  // Make follower visible again
						
						isFollowerRespawning = false;
					}
					continue;
				}

				// Normal following behavior
				int followerIndex = 1;
				for (int j = 0; j < 3; ++j) {
					if (j != i && characters[j] != currentPlayer) {
						followerIndex = 2;
						break;
					}
				}

				float targetX = mainX - (gap * followerIndex);
				float dx = targetX - characters[i]->getX();

				// Smooth following with proportional speed
				float followSpeed = min(characters[i]->getMaxSpeed() * 0.8f, abs(dx) * 0.2f);
				if (dx > 5.0f) {
					characters[i]->setVelX(followSpeed);
				}
				else if (dx < -5.0f) 
				{
					characters[i]->setVelX(-followSpeed);
				}
				else 
				{
					characters[i]->setVelX(dx * 0.5f);
				}

				// Synchronized jumping
				if (jumpCommand && characters[i]->isOnGround()) {
					characters[i]->setVelY(characters[i]->getJumpStrength());
					characters[i]->setOnGround(false);
				}

				characters[i]->updatePhysics(level);
				characters[i]->updateAbility(deltaTime);
			}
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