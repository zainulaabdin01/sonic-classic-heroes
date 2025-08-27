#ifndef LEVELS_H
#define LEVELS_H

#include "Level.h"
#include <fstream>
#include <iostream>

using namespace sf;
using namespace std;

class LabyrinthZone : public Level {
private:
    Texture labyrinthBackgroundTexture;
    Sprite labyrinthBackgroundSprite;
    Texture breakableWallTexture;
    Sprite breakableWallSprite;

public:
    LabyrinthZone(ScoreManager* scoreMgr, HealthManager* healthMgr) : Level(200, 14, 64.0f, scoreMgr, healthMgr) {
        physicsConfig = PhysicsConfig(0.2f, 15.0f, 0.1f, 0.2f, 0.98f, 20.0f, -20.0f); // Normal/default
        loadTextures();
        createLevel();
    }

    void loadTextures() override {
        // Load level-specific textures
        if (!wallTexture.loadFromFile("Data/brick2.png")) {
            cout << "Failed to load labyrinth wall texture" << endl;
        }
        if (!platformTexture.loadFromFile("Data/wall.png")) {
            cout << "Failed to load labyrinth platform texture" << endl;
        }
        if (!labyrinthBackgroundTexture.loadFromFile("Data/background.png")) {
            cout << "Failed to load labyrinth background texture" << endl;
        }
        if (!breakableWallTexture.loadFromFile("Data/brick3.png")) {
            cout << "Failed to load labyrinth breakable wall texture" << endl;
        }

        wallSprite.setTexture(wallTexture);
        platformSprite.setTexture(platformTexture);
        labyrinthBackgroundSprite.setTexture(labyrinthBackgroundTexture);
        breakableWallSprite.setTexture(breakableWallTexture);

        // Set sprite scales to match cell size
        wallSprite.setScale(
            static_cast<float>(cellSize) / wallTexture.getSize().x,
            static_cast<float>(cellSize) / wallTexture.getSize().y
        );
        platformSprite.setScale(
            static_cast<float>(cellSize) / platformTexture.getSize().x,
            static_cast<float>(cellSize) / platformTexture.getSize().y
        );
        labyrinthBackgroundSprite.setScale(
            static_cast<float>(BACKGROUND_WIDTH) / labyrinthBackgroundTexture.getSize().x,
            static_cast<float>(BACKGROUND_HEIGHT) / labyrinthBackgroundTexture.getSize().y
        );
        breakableWallSprite.setScale(
            static_cast<float>(cellSize) / breakableWallTexture.getSize().x,
            static_cast<float>(cellSize) / breakableWallTexture.getSize().y
        );
    }

    void draw(RenderWindow& window, float camera_offset_x) override {
        // Calculate how many background tiles we need to draw
        int startTile = static_cast<int>(camera_offset_x / BACKGROUND_WIDTH);
        int endTile = static_cast<int>((camera_offset_x + SCREEN_WIDTH) / BACKGROUND_WIDTH) + 1;

        // Draw background tiles
        for (int tile = startTile; tile <= endTile; tile++) {
            labyrinthBackgroundSprite.setPosition(
                tile * BACKGROUND_WIDTH - camera_offset_x,
                0
            );
            window.draw(labyrinthBackgroundSprite);
        }

        // Calculate visible cells
        int startCol = static_cast<int>(camera_offset_x / cellSize);
        int endCol = static_cast<int>((camera_offset_x + SCREEN_WIDTH) / cellSize) + 1;
        startCol = max(0, startCol);
        endCol = min(width - 1, endCol);

        // Draw walls, platforms, and breakable walls
        for (int i = 0; i < height; i++) {
            for (int j = startCol; j <= endCol; j++) {
                if (levelData[i][j] == 'w') {
                    wallSprite.setPosition(
                        j * cellSize - camera_offset_x,
                        i * cellSize
                    );
                    window.draw(wallSprite);
                }
                else if (levelData[i][j] == 'p') {
                    platformSprite.setPosition(
                        j * cellSize - camera_offset_x,
                        i * cellSize
                    );
                    window.draw(platformSprite);
                }
                else if (levelData[i][j] == 'b') {
                    breakableWallSprite.setPosition(
                        j * cellSize - camera_offset_x,
                        i * cellSize
                    );
                    window.draw(breakableWallSprite);
                }
            }
        }

        // Draw obstacles
        drawObstacles(window, camera_offset_x);

        // Draw collectibles (rings)
        drawCollectibles(window, camera_offset_x);
    }

    void createLevel() override {
        // Clear the level first
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                levelData[i][j] = 's';

        // Load from file using base class method
        if (!Level::loadLayoutFromFile("Data/level1.txt")) {
            cout << "Falling back to default layout!" << endl;
        }
        spawnRandomEnemies(8);

    }

    void reset() override {
        // Clear existing obstacles and collectibles
        for (int i = 0; i < obstacleCount; ++i) {
            delete obstacles[i];
        }
        obstacleCount = 0;
        for (int i = 0; i < collectibleCount; ++i) {
            delete collectibles[i];
        }
        collectibleCount = 0;
        
        initializeLevel();
        createLevel();
    }
};

class IceCapZone : public Level {
private:
    Texture iceBackgroundTexture;
    Sprite iceBackgroundSprite;
    Texture breakableWallTexture;
    Sprite breakableWallSprite;

public:
    IceCapZone(ScoreManager* scoreMgr, HealthManager* healthMgr) : Level(250, 14, 64.0f, scoreMgr, healthMgr) {
        physicsConfig = PhysicsConfig(0.5f, 15.0f, 0.02f, 0.5f, 0.98f, 20.0f, -20.0f); // Ice: high accel/decel, low friction
        loadTextures();
        createLevel();
    }

    void loadTextures() override {
        // Load level-specific textures
        if (!wallTexture.loadFromFile("Data/ice_wall.png")) {
            cout << "Failed to load ice wall texture" << endl;
        }
        if (!platformTexture.loadFromFile("Data/ice_platform.png")) {
            cout << "Failed to load ice platform texture" << endl;
        }
        if (!iceBackgroundTexture.loadFromFile("Data/ice_background.png")) {
            cout << "Failed to load ice background texture" << endl;
        }
        if (!breakableWallTexture.loadFromFile("Data/ice_breakable_wall.png")) {
            cout << "Failed to load ice breakable wall texture" << endl;
        }

        wallSprite.setTexture(wallTexture);
        platformSprite.setTexture(platformTexture);
        iceBackgroundSprite.setTexture(iceBackgroundTexture);
        breakableWallSprite.setTexture(breakableWallTexture);

        // Set sprite scales to match cell size
        wallSprite.setScale(
            static_cast<float>(cellSize) / wallTexture.getSize().x,
            static_cast<float>(cellSize) / wallTexture.getSize().y
        );
        platformSprite.setScale(
            static_cast<float>(cellSize) / platformTexture.getSize().x,
            static_cast<float>(cellSize) / platformTexture.getSize().y
        );
        iceBackgroundSprite.setScale(
            static_cast<float>(BACKGROUND_WIDTH) / iceBackgroundTexture.getSize().x,
            static_cast<float>(BACKGROUND_HEIGHT) / iceBackgroundTexture.getSize().y
        );
        breakableWallSprite.setScale(
            static_cast<float>(cellSize) / breakableWallTexture.getSize().x,
            static_cast<float>(cellSize) / breakableWallTexture.getSize().y
        );
    }

    void draw(RenderWindow& window, float camera_offset_x) override {
        // Calculate how many background tiles we need to draw
        int startTile = static_cast<int>(camera_offset_x / BACKGROUND_WIDTH);
        int endTile = static_cast<int>((camera_offset_x + SCREEN_WIDTH) / BACKGROUND_WIDTH) + 1;

        // Draw background tiles
        for (int tile = startTile; tile <= endTile; tile++) {
            iceBackgroundSprite.setPosition(
                tile * BACKGROUND_WIDTH - camera_offset_x,
                0
            );
            window.draw(iceBackgroundSprite);
        }

        // Calculate visible cells
        int startCol = static_cast<int>(camera_offset_x / cellSize);
        int endCol = static_cast<int>((camera_offset_x + SCREEN_WIDTH) / cellSize) + 1;
        startCol = max(0, startCol);
        endCol = min(width - 1, endCol);

        // Draw walls, platforms, and breakable walls
        for (int i = 0; i < height; i++) {
            for (int j = startCol; j <= endCol; j++) {
                if (levelData[i][j] == 'w') {
                    wallSprite.setPosition(
                        j * cellSize - camera_offset_x,
                        i * cellSize
                    );
                    window.draw(wallSprite);
                }
                else if (levelData[i][j] == 'p') {
                    platformSprite.setPosition(
                        j * cellSize - camera_offset_x,
                        i * cellSize
                    );
                    window.draw(platformSprite);
                }
                else if (levelData[i][j] == 'b') {
                    breakableWallSprite.setPosition(
                        j * cellSize - camera_offset_x,
                        i * cellSize
                    );
                    window.draw(breakableWallSprite);
                }
            }
        }

        drawObstacles(window, camera_offset_x);

        // Draw collectibles (rings)
        drawCollectibles(window, camera_offset_x);
    }

    void createLevel() override {
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                levelData[i][j] = 's';

        // Load from file using base class method
        if (!Level::loadLayoutFromFile("Data/level1.txt")) {
            cout << "Falling back to default layout!" << endl;
        }
        spawnRandomEnemies(12);
        //loadMusic("Data/level2.ogg");
    }

    void reset() override {
        // Clear existing obstacles and collectibles
        for (int i = 0; i < obstacleCount; ++i) {
            delete obstacles[i];
        }
        obstacleCount = 0;
        for (int i = 0; i < collectibleCount; ++i) {
            delete collectibles[i];
        }
        collectibleCount = 0;
        
        initializeLevel();
        createLevel();
    }
};

class DeathEggZone : public Level {
private:
    Texture deathEggBackgroundTexture;
    Sprite deathEggBackgroundSprite;
    Texture breakableWallTexture;
    Sprite breakableWallSprite;

public:
    DeathEggZone(ScoreManager* scoreMgr, HealthManager* healthMgr) : Level(300, 14, 64.0f, scoreMgr, healthMgr) {
        physicsConfig = PhysicsConfig(0.08f, 15.0f, 0.05f, 0.08f, 0.5f, 20.0f, -20.0f); // Low gravity, low accel/decel, low friction
        loadTextures();
        createLevel();
    }

    void loadTextures() override {
        // Load level-specific textures
        if (!wallTexture.loadFromFile("Data/deathegg_brick.png")) {
            cout << "Failed to load death egg wall texture" << endl;
        }
        if (!platformTexture.loadFromFile("Data/deathegg_platform.png")) {
            cout << "Failed to load death egg platform texture" << endl;
        }
        if (!deathEggBackgroundTexture.loadFromFile("Data/deathegg_background.png")) {
            cout << "Failed to load death egg background texture" << endl;
        }
        if (!breakableWallTexture.loadFromFile("Data/death_breakable_wall.png")) {
            cout << "Failed to load death egg breakable wall texture" << endl;
        }
        wallSprite.setTexture(wallTexture);
        platformSprite.setTexture(platformTexture);
        deathEggBackgroundSprite.setTexture(deathEggBackgroundTexture);
        breakableWallSprite.setTexture(breakableWallTexture);

        // Set sprite scales to match cell size
        wallSprite.setScale(
            static_cast<float>(cellSize) / wallTexture.getSize().x,
            static_cast<float>(cellSize) / wallTexture.getSize().y
        );
        platformSprite.setScale(
            static_cast<float>(cellSize) / platformTexture.getSize().x,
            static_cast<float>(cellSize) / platformTexture.getSize().y
        );
        deathEggBackgroundSprite.setScale(
            static_cast<float>(BACKGROUND_WIDTH) / deathEggBackgroundTexture.getSize().x,
            static_cast<float>(BACKGROUND_HEIGHT) / deathEggBackgroundTexture.getSize().y
        );
        breakableWallSprite.setScale(
            static_cast<float>(cellSize) / breakableWallTexture.getSize().x,
            static_cast<float>(cellSize) / breakableWallTexture.getSize().y
        );
    }

    void draw(RenderWindow& window, float camera_offset_x) override {
        // Calculate how many background tiles we need to draw
        int startTile = static_cast<int>(camera_offset_x / BACKGROUND_WIDTH);
        int endTile = static_cast<int>((camera_offset_x + SCREEN_WIDTH) / BACKGROUND_WIDTH) + 1;

        // Draw background tiles
        for (int tile = startTile; tile <= endTile; tile++) {
            deathEggBackgroundSprite.setPosition(
                tile * BACKGROUND_WIDTH - camera_offset_x,
                0
            );
            window.draw(deathEggBackgroundSprite);
        }

        // Calculate visible cells
        int startCol = static_cast<int>(camera_offset_x / cellSize);
        int endCol = static_cast<int>((camera_offset_x + SCREEN_WIDTH) / cellSize) + 1;
        startCol = max(0, startCol);
        endCol = min(width - 1, endCol);

        // Draw walls, platforms, and breakable walls
        for (int i = 0; i < height; i++) {
            for (int j = startCol; j <= endCol; j++) {
                if (levelData[i][j] == 'w') {
                    wallSprite.setPosition(
                        j * cellSize - camera_offset_x,
                        i * cellSize
                    );
                    window.draw(wallSprite);
                }
                else if (levelData[i][j] == 'p') {
                    platformSprite.setPosition(
                        j * cellSize - camera_offset_x,
                        i * cellSize
                    );
                    window.draw(platformSprite);
                }
                else if (levelData[i][j] == 'b') {
                    breakableWallSprite.setPosition(
                        j * cellSize - camera_offset_x,
                        i * cellSize
                    );
                    window.draw(breakableWallSprite);
                }
            }
        }
        drawObstacles(window, camera_offset_x);

        // Draw collectibles (rings)
        drawCollectibles(window, camera_offset_x);
    }

    void createLevel() override {
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                levelData[i][j] = 's';

        // Load from file using base class method
        if (!Level::loadLayoutFromFile("Data/level1.txt")) {
            cout << "Falling back to default layout!" << endl;
            // Optionally, call your old hardcoded layout here
        }
        spawnRandomEnemies(16);
        //loadMusic("Data/level3.ogg");
    }

    void reset() override {
        // Clear existing obstacles and collectibles
        for (int i = 0; i < obstacleCount; ++i) {
            delete obstacles[i];
        }
        obstacleCount = 0;
        for (int i = 0; i < collectibleCount; ++i) {
            delete collectibles[i];
        }
        collectibleCount = 0;
        
        initializeLevel();
        createLevel();
    }
};

#endif 