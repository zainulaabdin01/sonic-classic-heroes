#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>
#include <cmath>

using namespace sf;
using namespace std;

// Collision helper function
bool checkCollision(float x1, float y1, float w1, float h1,
    float x2, float y2, float w2, float h2) {
    return x1 < x2 + w2 &&
        x1 + w1 > x2 &&
        y1 < y2 + h2 &&
        y1 + h1 > y2;
}

// Base Enemy Class
class Enemy {
protected:
    Sprite sprite;
    Texture texture;
    float posX, posY;
    float width, height;
    int health;
    float speed;
    bool isAlive;

    bool loadTexture(const string& path) {
        return texture.loadFromFile(path);
    }

public:
    Enemy() : isAlive(true) {}
    virtual ~Enemy() = default;

    virtual void update(float deltaTime, float playerX, float playerY) = 0;
    virtual void draw(RenderWindow& window, float camera_offset_x) {
        if (isAlive) {
            sprite.setPosition(posX - camera_offset_x, posY);
            window.draw(sprite);
        }
    }

    bool getIsAlive() const { return isAlive; }
    void getPosition(float& x, float& y) const { x = posX; y = posY; }
    void getSize(float& w, float& h) const { w = width; h = height; }

    void takeDamage(int damage) {
        health -= damage;
        if (health <= 0) isAlive = false;
    }
};

#endif // ENEMY_H 