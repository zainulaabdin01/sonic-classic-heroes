#ifndef COLLECTIBLE_H
#define COLLECTIBLE_H

#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;

class Collectible {
protected:
    float x, y;
    float scale;
    bool isCollected;
    bool isVisible;
    Texture texture;
    Sprite sprite;
    int width, height;
    int hitBoxFactorX, hitBoxFactorY;

    // Non-virtual helper functions
    bool loadTexture(const string& filename) {
        if (!texture.loadFromFile(filename)) {
            cout << "Failed to load collectible texture: " << filename << endl;
            return false;
        }
        sprite.setTexture(texture);
        return true;
    }

    void updateSprite() {
        sprite.setPosition(x, y);
        sprite.setScale(scale, scale);
    }

public:
    Collectible(float startX, float startY, float scale = 1.0f) 
        : x(startX), y(startY), scale(scale), isCollected(false), isVisible(true) {
        hitBoxFactorX = 5;
        hitBoxFactorY = 5;
    }

    virtual ~Collectible() = default;

    // Non-virtual functions
    void setPosition(float newX, float newY) {
        x = newX;
        y = newY;
        updateSprite();
    }

    void setVisible(bool visible) {
        isVisible = visible;
    }

    bool getVisible() const {
        return isVisible;
    }

    bool isCollectedState() const {
        return isCollected;
    }

    void reset() {
        isCollected = false;
        isVisible = true;
    }

    // Virtual functions that must be implemented by derived classes
    virtual void update(float deltaTime) = 0;
    virtual void draw(RenderWindow& window, float camera_offset_x) = 0;
    virtual void onCollect() = 0;
    virtual bool checkCollision(float playerX, float playerY, int playerWidth, int playerHeight) = 0;
    virtual string getType() const = 0;

    // Virtual functions with default implementations
    virtual void setScale(float newScale) {
        scale = newScale;
        updateSprite();
    }

    virtual float getX() const { return x; }
    virtual float getY() const { return y; }
    virtual int getWidth() const { return width; }
    virtual int getHeight() const { return height; }
};

#endif // COLLECTIBLE_H 