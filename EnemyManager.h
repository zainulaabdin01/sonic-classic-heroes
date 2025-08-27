#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include "Enemy.h"
#include "BatBrain.h"
#include "BeeBot.h"
#include "Motobug.h"
#include "CrabMeat.h"

class EnemyManager {
private:
    static const int MAX_ENEMIES = 64;
    Enemy* enemies[MAX_ENEMIES];
    int enemyCount;

public:
    EnemyManager() : enemyCount(0) {
        for (int i = 0; i < MAX_ENEMIES; ++i) enemies[i] = nullptr;
    }
    ~EnemyManager() {
        clear();
    }

    void clear() {
        for (int i = 0; i < enemyCount; ++i) {
            delete enemies[i];
            enemies[i] = nullptr;
        }
        enemyCount = 0;
    }

    bool addBatBrain(float x, float y) {
        if (enemyCount >= MAX_ENEMIES) return false;
        enemies[enemyCount++] = new BatBrain(x, y);
        return true;
    }
    bool addBeeBot(float x, float y) {
        if (enemyCount >= MAX_ENEMIES) return false;
        enemies[enemyCount++] = new BeeBot(x, y);
        return true;
    }
    bool addMotobug(float x, float y) {
        if (enemyCount >= MAX_ENEMIES) return false;
        enemies[enemyCount++] = new Motobug(x, y);
        return true;
    }
    bool addCrabMeat(float x, float y) {
        if (enemyCount >= MAX_ENEMIES) return false;
        enemies[enemyCount++] = new CrabMeat(x, y);
        return true;
    }

    void updateAll(float deltaTime, float playerX, float playerY) {
        for (int i = 0; i < enemyCount; ++i) {
            if (enemies[i] && enemies[i]->getIsAlive()) {
                enemies[i]->update(deltaTime, playerX, playerY);
            }
        }
    }

    void drawAll(RenderWindow& window, float camera_offset_x) {
        for (int i = 0; i < enemyCount; ++i) {
            if (enemies[i] && enemies[i]->getIsAlive()) {
                enemies[i]->draw(window, camera_offset_x);
            }
        }
    }

    int getEnemyCount() const { return enemyCount; }
    Enemy* getEnemy(int idx) const { return (idx >= 0 && idx < enemyCount) ? enemies[idx] : nullptr; }
};

#endif // ENEMY_MANAGER_H 