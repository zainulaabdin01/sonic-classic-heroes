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
        scoreText.setFont(font);
        scoreText.setCharacterSize(32);
        scoreText.setFillColor(Color::Yellow);
        scoreText.setPosition(20, 20);
        healthText.setFont(font);
        healthText.setCharacterSize(28);
        healthText.setFillColor(Color::Red);
        healthText.setPosition(20, 60);
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

            // Handle input only if not in transition
            if (!levelManager.isInTransition()) {
                playerManager.handleInput(levelManager.getCurrentLevel());
            }

            // Update physics only if not in transition
            if (!levelManager.isInTransition()) {
                playerManager.updatePhysics(levelManager.getCurrentLevel());
            }

            // Check for level transition
            if (playerManager.getCurrentPlayer()->needsLevelTransition()) {
                levelManager.handleLevelTransition(playerManager.getCurrentPlayer());
            }

            // Update transition state
            if (levelManager.updateTransition(playerManager.getCurrentPlayer())) {
                camera_offset_x = 0;
            }

            // Update camera position only if not in transition
            if (!levelManager.isInTransition()) {
                float playerX = playerManager.getCurrentPlayer()->getX();
                if (playerX > 1200 / 2) {
                    camera_offset_x = playerX - 1200 / 2;
                }
            }

            // Update collectibles (for ring animation)
            float deltaTime = deltaClock.restart().asSeconds();
            levelManager.getCurrentLevel()->updateCollectibles(deltaTime);

            // Update enemies
            float playerX = playerManager.getCurrentPlayer()->getX();
            float playerY = playerManager.getCurrentPlayer()->getY();
            levelManager.getCurrentLevel()->updateEnemies(deltaTime, playerX, playerY);

            // Check for enemy or projectile collision and apply damage
            Player* mainPlayer = playerManager.getCurrentPlayer();
            if (!mainPlayer->getIsInvulnerable() && levelManager.getCurrentLevel()->checkEnemyCollisions(playerX, playerY, mainPlayer->getWidth(), mainPlayer->getHeight())) {
                //healthManager.decrementHealth();
                //mainPlayer->takeDamage();
            }
            mainPlayer->updateInvulnerability();

            // Update score text
            scoreText.setString("Score: " + std::to_string(scoreManager.getScore()));
            // Update health text
            healthText.setString("Health: " + std::to_string(healthManager.getHealth()));

            // Draw everything
            window.clear(Color::White);
            levelManager.drawLevel(window, camera_offset_x);
            levelManager.getCurrentLevel()->drawEnemies(window, camera_offset_x);
            playerManager.draw(window, camera_offset_x);
            window.draw(scoreText);
            window.draw(healthText);
            window.display();

            // Close the window if the game is over
            if (Player::isGameOverState()) {
                window.close();
            }
        }
    }
};