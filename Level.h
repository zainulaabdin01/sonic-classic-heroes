#ifndef LEVEL_H
#define LEVEL_H

#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include "Obstacle.h"
#include "Spike.h"
#include "PhysicsConfig.h"
#include "Collectible.h"
#include "Ring.h"
#include "ExtraLife.h"
#include "ScoreManager.h"
#include "HealthManager.h"
#include "SpecialBoost.h"
#include "EnemyManager.h"
#include <SFML/Audio.hpp>

using namespace sf;
using namespace std;

class Level {
protected:
    char** levelData;
    int width;
    int height;
    float cellSize;
    const int SCREEN_WIDTH = 1200;
    const int SCREEN_HEIGHT = 900;
    const int BACKGROUND_WIDTH = 1600;
    const int BACKGROUND_HEIGHT = 900;
    static const int MAX_OBSTACLES = 256;
    Obstacle* obstacles[MAX_OBSTACLES];
    int obstacleCount;
    static const int MAX_COLLECTIBLES = 256;
    Collectible* collectibles[MAX_COLLECTIBLES];
    int collectibleCount;
    Texture wallTexture;
    Texture platformTexture;
    Sprite wallSprite;
    Sprite platformSprite;
    PhysicsConfig physicsConfig;
    ScoreManager* scoreManager;
    HealthManager* healthManager;
    EnemyManager enemyManager;
    Music music;
    static Music* currentMusic;

public:
    Level(int w, int h, float cellSize, ScoreManager* scoreMgr, HealthManager* healthMgr) : width(w), height(h), cellSize(cellSize), scoreManager(scoreMgr), healthManager(healthMgr) {
        initializeLevel();
    }

    virtual ~Level() {
        // Clean up level data
        if (levelData) {
            for (int i = 0; i < height; i++) {
                delete[] levelData[i];
            }
            delete[] levelData;
        }
        // Clean up obstacles
        for (int i = 0; i < obstacleCount; i++) {
            delete obstacles[i];
        }
        obstacleCount = 0;
        // Clean up collectibles
        for (int i = 0; i < collectibleCount; i++) {
            delete collectibles[i];
        }
        collectibleCount = 0;
        enemyManager.clear();
    }

    // Pure virtual methods that must be implemented by derived classes
    virtual void createLevel() = 0;
    virtual void reset() = 0;
    virtual void draw(RenderWindow& window, float camera_offset_x) = 0;
    virtual void loadTextures() = 0;

    // Add ring to the level
    void addRing(int gridX, int gridY) {
        if (gridX >= 0 && gridX < width && gridY >= 0 && gridY < height) {
            levelData[gridY][gridX] = 'r';  // 'r' represents ring
            collectibles[collectibleCount++] = new Ring(gridX * cellSize, gridY * cellSize, scoreManager);
        }
    }

    // Draw collectibles
    void drawCollectibles(RenderWindow& window, float camera_offset_x) {
        for (int i = 0; i < collectibleCount; i++) {
            if (collectibles[i]->getVisible()) {
                collectibles[i]->draw(window, camera_offset_x);
            }
        }
    }

    // Update collectibles
    void updateCollectibles(float deltaTime) {
        for (int i = 0; i < collectibleCount; i++) {
            collectibles[i]->update(deltaTime);
        }
    }

    // Check collectible collisions
    void checkCollectibleCollisions(float playerX, float playerY, float playerWidth, float playerHeight) {
        for (int i = 0; i < collectibleCount; i++) {
            if (collectibles[i]->checkCollision(playerX, playerY, playerWidth, playerHeight)) {
                collectibles[i]->onCollect();
                // Set the corresponding cell to empty space
                int gridX = static_cast<int>(collectibles[i]->getX() / cellSize);
                int gridY = static_cast<int>(collectibles[i]->getY() / cellSize);
                if (gridX >= 0 && gridX < width && gridY >= 0 && gridY < height) {
                    levelData[gridY][gridX] = 's';
                }
            }
        }
    }

    // Check if position is in the last pit
    bool isInLastPit(float x, float y) const {
        int cell_x = static_cast<int>(x / cellSize);
        int cell_y = static_cast<int>(y / cellSize);
        
        // Check if position is within bounds
        if (cell_x < 0 || cell_x >= width || cell_y < 0 || cell_y >= height) {
            return false;
        }

        // Check if position is in the last 10 cells of the level
        return cell_x >= (width - 10) && levelData[cell_y][cell_x] == 's';
    }

    // Common methods that can be used by all levels
    void initializeLevel() {
        // Allocate memory for level data
        levelData = new char*[height];
        for (int i = 0; i < height; i++) {
            levelData[i] = new char[width];
            for (int j = 0; j < width; j++) {
                levelData[i][j] = 's';  // Initialize with empty space
            }
        }
    }

    // Add wall to the level
    void addWall(int gridX, int gridY) {
        if (gridX >= 0 && gridX < width && gridY >= 0 && gridY < height) {
            levelData[gridY][gridX] = 'w';  // 'w' represents wall
        }
    }

    // Add platform to the level
    void addPlatform(int gridX, int gridY) {
        if (gridX >= 0 && gridX < width && gridY >= 0 && gridY < height) {
            levelData[gridY][gridX] = 'p';  // 'p' represents platform
        }
    }

    // Add spike to the level
    void addSpike(int gridX, int gridY) {
        if (gridX >= 0 && gridX < width && gridY >= 0 && gridY < height) {
            levelData[gridY][gridX] = 'o';  // 'o' represents obstacle
            obstacles[obstacleCount++] = new Spike(gridX * cellSize, gridY * cellSize, cellSize);
        }
    }

    // Draw obstacles
    void drawObstacles(RenderWindow& window, float camera_offset_x) {
        for (int i = 0; i < obstacleCount; i++) {
            obstacles[i]->draw(window, camera_offset_x);        
        }
    }

    // Check obstacle collisions
    bool checkObstacleCollisions(float playerX, float playerY, float playerWidth, float playerHeight) {
        for (int i = 0; i < obstacleCount; i++) {
            if (obstacles[i]->checkCollision(playerX, playerY, playerWidth, playerHeight)) {
                if (static_cast<Spike*>(obstacles[i]) != nullptr) {
                    return true;  // For spikes, return true
                }
                return true;  // For other obstacles, just return true
            }
        }
        return false;
    }

    // Getters
    char** getLevelData() const { return levelData; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    float getCellSize() const { return cellSize; }
    const Obstacle* const* getObstacles() const { return obstacles; }
    Obstacle** getObstacles() { return obstacles; }
    int getObstacleCount() const { return obstacleCount; }
    const Collectible* const* getCollectibles() const { return collectibles; }
    PhysicsConfig* getPhysicsConfig() { return &physicsConfig; }

    // Add extra life to the level
    void addExtraLife(int gridX, int gridY) {
        if (gridX >= 0 && gridX < width && gridY >= 0 && gridY < height) {
            levelData[gridY][gridX] = 'l';  // 'l' represents extra life
            collectibles[collectibleCount++] = new ExtraLife(gridX * cellSize, gridY * cellSize, healthManager);
        }
    }

    // Add special boost to the level
    void addSpecialBoost(int gridX, int gridY) {
        if (gridX >= 0 && gridX < width && gridY >= 0 && gridY < height) {
            levelData[gridY][gridX] = 'z';  // 'z' represents special boost
            collectibles[collectibleCount++] = new SpecialBoost(gridX * cellSize, gridY * cellSize);
        }
    }

    // Enemy management
    EnemyManager* getEnemyManager() { return &enemyManager; }
    void addBatBrain(int gridX, int gridY) { enemyManager.addBatBrain(gridX * cellSize, gridY * cellSize); }
    void addBeeBot(int gridX, int gridY) { enemyManager.addBeeBot(gridX * cellSize, gridY * cellSize); }
    void addMotobug(int gridX, int gridY) { enemyManager.addMotobug(gridX * cellSize, gridY * cellSize); }
    void addCrabMeat(int gridX, int gridY) { enemyManager.addCrabMeat(gridX * cellSize, gridY * cellSize); }
    void updateEnemies(float deltaTime, float playerX, float playerY) { enemyManager.updateAll(deltaTime, playerX, playerY); }
    void drawEnemies(RenderWindow& window, float camera_offset_x) { enemyManager.drawAll(window, camera_offset_x); }

	//Spawn random enemies
    void spawnRandomEnemies(int count) {
        srand(static_cast<unsigned>(time(nullptr)));
        int attempts = 0;
        int spawned = 0;
        while (spawned < count && attempts < count * 10) { 
            int gx = rand() % width;
            int gy = rand() % (height - 1); 
            if (levelData[gy][gx] == 's') {
                int type = rand() % 4;
                bool canSpawn = true;
                if (type == 2 || type == 3) { 
                    if (gy + 1 >= height || !(levelData[gy + 1][gx] == 'w' || levelData[gy + 1][gx] == 'p')) {
                        canSpawn = false;
                    }
                }
                if (canSpawn) {
                    switch (type) {
                        case 0: addBatBrain(gx, gy); break;
                        case 1: addBeeBot(gx, gy); break;
                        case 2: addMotobug(gx, gy); break;
                        case 3: addCrabMeat(gx, gy); break;
                    }
                    spawned++;
                }
            }
            attempts++;
        }
    }

	// Check for enemy collisions
    bool checkEnemyCollisions(float playerX, float playerY, float playerWidth, float playerHeight) {

        for (int i = 0; i < enemyManager.getEnemyCount(); ++i) {
            Enemy* enemy = enemyManager.getEnemy(i);
            if (enemy && enemy->getIsAlive()) {
                float ex, ey, ew, eh;
                enemy->getPosition(ex, ey);
                enemy->getSize(ew, eh);
                if (checkCollision(playerX, playerY, playerWidth, playerHeight, ex, ey, ew, eh)) {
                    return true;
                }
                BeeBot* bee = static_cast<BeeBot*>(enemy);
                if (bee) {
                    for (int j = 0; j < 5; ++j) {
                        const Projectile& p = bee->getProjectile(j);
                        if (p.active && checkCollision(playerX, playerY, playerWidth, playerHeight, p.x, p.y, Projectile::SIZE, Projectile::SIZE)) {
                            return true;
                        }
                    }
                }
                CrabMeat* crab = static_cast<CrabMeat*>(enemy);
                if (crab) {
                    for (int j = 0; j < 4; ++j) {
                        const CrabMeat::Projectile& p = crab->getProjectile(j);
                        if (p.active && checkCollision(playerX, playerY, playerWidth, playerHeight, p.x, p.y, 10.0f, 6.0f)) {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    void loadMusic(const string& filename) {
        if (currentMusic && currentMusic->getStatus() == Music::Playing) {
            currentMusic->stop();
        }
        if (music.openFromFile(filename)) {
            music.setLoop(true);
            music.play();
            currentMusic = &music;
        }
    }

    void stopMusic() {
        music.stop();
    }

protected:
    bool loadLayoutFromFile(const char* filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Failed to open level file: " << filename << std::endl;
            return false;
        }
        char line[256];
        int row = 0;
        while (file.getline(line, sizeof(line)) && row < height) {
            for (int col = 0; col < width && line[col] != '\0'; ++col) {
                char c = line[col];
                switch (c) {
                    case 'w': addWall(col, row); break;
                    case 'p': addPlatform(col, row); break;
                    case 'b': levelData[row][col] = 'b'; break;
                    case 'o': addSpike(col, row); break;
                    case 'r': addRing(col, row); levelData[row][col] = 's'; break;
                    case 'l': addExtraLife(col, row); levelData[row][col] = 's'; break;
                    case 'z': addSpecialBoost(col, row); levelData[row][col] = 's'; break;
                    // 's' is space, do nothing
                    default: levelData[row][col] = c; break;
                }
            }
            row++;
        }
        file.close();
        return true;
    }
};

// Initialize static member
Music* Level::currentMusic = nullptr;

#endif 