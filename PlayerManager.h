#pragma once
#include "Player.h"
class PlayerManager
{
	Player* characters[3];
	Player* currentPlayer;
	const float gap = 50.0f;
	Clock abilityClock;
public:
	PlayerManager() {
		characters[0] = new Sonic(100, 100);
		characters[1] = new Tails(100 - gap, 100);
		characters[2] = new Knuckles(100 - 2 * gap , 100);
		currentPlayer = characters[0]; // Start with Sonic
		abilityClock.restart();
	}

	~PlayerManager()
	{
		for (int i = 0; i < 3; ++i)
		{
			delete characters[i];
		}
	}

	void switchCharacter() //Switch character
	{
		// Store current position and velocity
		float x = currentPlayer->getX();
		float y = currentPlayer->getY();
		float vx = currentPlayer->getVelX();
		float vy = currentPlayer->getVelY();

		// Find current character index
		int currentIndex = 0;
		for (int i = 0; i < 3; ++i) {
			if (currentPlayer == characters[i]) {
				currentIndex = i;
				break;
			}
		}

		// Switch to next character
		currentIndex = (currentIndex + 1) % 3;
		currentPlayer = characters[currentIndex];

		// Transfer position and velocity
		currentPlayer->setPosition(x, y);
		currentPlayer->setVelocity(vx, vy);

		// Add a small cooldown to prevent rapid switching
		static Clock switchCooldown;
		if (switchCooldown.getElapsedTime().asSeconds() < 0.5f) {
			return;  // Don't switch if cooldown is active
		}
		switchCooldown.restart();
	}

	void handleInput(char** lvl, int cell_size)
	{
		static Clock switchCooldown;

		currentPlayer->handleInput();

		if (Keyboard::isKeyPressed(Keyboard::Tab) &&
			switchCooldown.getElapsedTime().asSeconds() > 0.5f) {
			switchCharacter();
			switchCooldown.restart();
		}

		if (Keyboard::isKeyPressed(Keyboard::LControl)) {
			currentPlayer->activateAbility(lvl, cell_size);
		}
	}

	void updatePhysics(char** lvl, int cell_size)
	{
		float deltaTime = abilityClock.restart().asSeconds();
		
		bool jumpCommand = currentPlayer->hasJustJumped();
		currentPlayer->updatePhysics(lvl, cell_size);
		currentPlayer->updateAbility(deltaTime);
		currentPlayer->resetJumpFlag();

		float mainX = currentPlayer->getX();
		float mainY = currentPlayer->getY();

		// Update followers
		for (int i = 0; i < 3; ++i) {
			if (characters[i] != currentPlayer) {
				// Following logic with gap
				int followerIndex = 1;
				for (int j = 0; j < 3; ++j) {
					if (j != i && characters[j] != currentPlayer) {
						followerIndex = 2; // This is the second follower
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
					characters[i]->setVelX(dx * 0.5f); // Gentle adjustment when close
				}

				// Synchronized jumping
				if (jumpCommand && characters[i]->isOnGround()) {
					characters[i]->setVelY(characters[i]->getJumpStrength());
					characters[i]->setOnGround(false);
				}

				characters[i]->updatePhysics(lvl, cell_size);
				characters[i]->updateAbility(deltaTime);

				// Respawn logic if character falls off
				if (characters[i]->getY() > 800) {
					characters[i]->setPosition(mainX - (gap * followerIndex), mainY);
					characters[i]->setVelocity(0, 0);
				}
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

	float getPlayerX() const { return currentPlayer->getX(); }
};