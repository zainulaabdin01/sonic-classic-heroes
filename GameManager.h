#pragma once
#include <SFML/Graphics.hpp>
#include "ScoreManager.h"
#include "HealthManager.h"
#include "PlayerManager.h"
#include "LevelManager.h"
#include "menu.h"

using namespace sf;

class GameManager {
private:
    RenderWindow window;
    ScoreManager scoreManager;
    HealthManager healthManager;
    PlayerManager playerManager;
    LevelManager levelManager;
    float camera_offset_x;
    Font font;
    Text scoreText;
    Text healthText;
    Text levelText;
    Clock deltaClock;
    int startLevelIndex;

public:
    GameManager(int startLevelIndex_ = 1)
        : window(VideoMode(1200, 900), "Sonic Game"),
          playerManager(&healthManager),
          levelManager(&playerManager, &scoreManager, &healthManager),
          camera_offset_x(0),
          startLevelIndex(startLevelIndex_)
    {
        window.setFramerateLimit(60);
        if (!font.loadFromFile("Data/Gaslight_Regular.ttf")) {
            // Handle error (font not found)
        }
        // Score text - left side
        scoreText.setFont(font);
        scoreText.setCharacterSize(38);
        scoreText.setFillColor(Color::Yellow);
        scoreText.setPosition(20, 20);
        
        // Health text - left side below score
        healthText.setFont(font);
        healthText.setCharacterSize(34);
        healthText.setFillColor(Color::Red);
        healthText.setPosition(20, 65);
        
        // Level text - right side
        levelText.setFont(font);
        levelText.setCharacterSize(38);
        levelText.setFillColor(Color::Cyan);
        levelText.setPosition(1050, 20);
    }

    void run() {
        // Show menu first
        // (Menu now handled in Source.cpp, so just set level)
        levelManager.setCurrentLevelIndex(startLevelIndex - 1); // 0-based
        // Main game loop
        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();
            }

            // Get current player with null check
            Player* currentPlayer = playerManager.getCurrentPlayer();
            Level* currentLevel = levelManager.getCurrentLevel();
            
            if (!currentPlayer || !currentLevel) {
                continue;  // Skip frame if player or level is null
            }

            // Handle input only if not in transition
            if (!levelManager.isInTransition()) {
                playerManager.handleInput(currentLevel);
            }

            // Update physics only if not in transition
            if (!levelManager.isInTransition()) {
                playerManager.updatePhysics(currentLevel);
            }

            // Check for level transition
            if (currentPlayer->needsLevelTransition()) {
                levelManager.handleLevelTransition(currentPlayer);
            }

            // Update transition state
            if (levelManager.updateTransition(currentPlayer)) {
                camera_offset_x = 0;
            }

            // Update camera position only if not in transition
            if (!levelManager.isInTransition()) {
                float playerX = currentPlayer->getX();
                if (playerX > 1200 / 2) {
                    camera_offset_x = playerX - 1200 / 2;
                }
            }

            // Update collectibles (for ring animation)
            float deltaTime = deltaClock.restart().asSeconds();
            currentLevel->updateCollectibles(deltaTime);

            // Update enemies
            float playerX = currentPlayer->getX();
            float playerY = currentPlayer->getY();
            currentLevel->updateEnemies(deltaTime, playerX, playerY);

            // Check for enemy or projectile collision and apply damage
            if (!currentPlayer->getIsInvulnerable() && currentLevel->checkEnemyCollisions(playerX, playerY, currentPlayer->getWidth(), currentPlayer->getHeight())) {
                //healthManager.decrementHealth();
                //currentPlayer->takeDamage();
            }
            currentPlayer->updateInvulnerability();

            // Update score text
            scoreText.setString("Score: " + std::to_string(scoreManager.getScore()));
            // Update health text
            healthText.setString("Health: " + std::to_string(healthManager.getHealth()));
            // Update level text
            levelText.setString("Level: " + std::to_string(levelManager.getCurrentLevelIndex() + 1));

            // Draw everything
            window.clear(Color::White);
            levelManager.drawLevel(window, camera_offset_x);
            levelManager.getCurrentLevel()->drawEnemies(window, camera_offset_x);
            playerManager.draw(window, camera_offset_x);
            window.draw(scoreText);
            window.draw(healthText);
            window.draw(levelText);
            window.display();

            // Close the window if the game is over
            if (Player::isGameOverState()) {
                window.close();
            }
        }
    }
};