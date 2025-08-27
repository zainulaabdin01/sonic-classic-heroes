#pragma once
#include <SFML/Graphics.hpp>
#include<iostream>

using namespace sf;
using namespace std;

class Obstacle {
protected:
    float x, y;
    float width, height;

public:
    Obstacle(float x, float y, float width, float height)
        : x(x), y(y), width(width), height(height) {}

    virtual ~Obstacle() = default;

    // Pure virtual methods that must be implemented by derived classes
    virtual void update(float deltaTime) = 0;
    virtual void draw(RenderWindow& window, float camera_offset_x) = 0;
    virtual bool checkCollision(float playerX, float playerY, float playerWidth, float playerHeight) = 0;

    // Getters
    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
}; 