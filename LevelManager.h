#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include <SFML/Graphics.hpp>
#include "Levels.h"
#include "PlayerManager.h"
#include <iostream>

using namespace sf;
using namespace std;

class LevelManager {
private:
    Level* levels[3];  
    int currentLevelIndex;
    const float CELL_SIZE = 64.0f;
    const float START_X = 100.0f; 
    const float START_Y = 100.0f;
    
    // Transition system
    Clock transitionClock;
    const float TRANSITION_DELAY = 1.0f;  
    bool isTransitioning;
    int nextLevelIndex;
    PlayerManager* playerManager; 

public:
    LevelManager(PlayerManager* pm, ScoreManager* scoreMgr, HealthManager* healthMgr) : currentLevelIndex(0), isTransitioning(false), nextLevelIndex(-1), playerManager(pm) {
        // Initialize levels
        levels[0] = new LabyrinthZone(scoreMgr, healthMgr);
        levels[1] = new IceCapZone(scoreMgr, healthMgr);
        levels[2] = new DeathEggZone(scoreMgr, healthMgr);
    }

    // Clean up levels
    ~LevelManager() {
        for (int i = 0; i < 3; i++) {
            delete levels[i];
        }
    }

    Level* getCurrentLevel() const {
        return levels[currentLevelIndex];
    }

    void nextLevel() {
        if (currentLevelIndex < 2) {  
            currentLevelIndex++;
            levels[currentLevelIndex]->reset();
        }
    }

    void previousLevel() {
        if (currentLevelIndex > 0) {
            currentLevelIndex--;
            levels[currentLevelIndex]->reset();
        }
    }

    void setLevel(int index) {
        if (index >= 0 && index < 3) {
            currentLevelIndex = index;
            levels[currentLevelIndex]->reset();
        }
    }

    int getCurrentLevelIndex() const {
        return currentLevelIndex;
    }

    // Common level functions
    void drawLevel(RenderWindow& window, float camera_offset_x) {
        levels[currentLevelIndex]->draw(window, camera_offset_x);
    }

    void resetLevel() {
        levels[currentLevelIndex]->reset();
    }

    // Handle level transition
    void handleLevelTransition(Player* player) {
        if (player->needsLevelTransition() && !isTransitioning) {
            isTransitioning = true;
            nextLevelIndex = currentLevelIndex + 1;
            transitionClock.restart();
            player->resetLevelTransition();
        }
    }

    // Update transition state
    bool updateTransition(Player* player) {
        if (isTransitioning) {
            if (transitionClock.getElapsedTime().asSeconds() >= TRANSITION_DELAY) {
                if (nextLevelIndex < 3) {
                    currentLevelIndex = nextLevelIndex;
                    levels[currentLevelIndex]->reset();
                    cout << "[DEBUG] Transitioned to level " << (currentLevelIndex + 1) << endl;
                    playerManager->resetAllPlayers(START_X, START_Y);
                }
                isTransitioning = false;
                nextLevelIndex = -1;
                return true;
            }
        }
        return false;
    }

    bool isInTransition() const {
        return isTransitioning;
    }

    void getStartPosition(float& x, float& y) const {
        x = START_X;
        y = START_Y;
    }

    void setCurrentLevelIndex(int idx) {
        if (idx >= 0 && idx < 3) {
            currentLevelIndex = idx;
            levels[currentLevelIndex]->reset();
        }
    }
};

#endif 