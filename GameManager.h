#pragma once
#include "Player.h"
#include <SFML/Graphics.hpp>
using namespace sf;

class GameManager {
private:
    RenderWindow window;
    Sonic sonic;
    Texture sonicTex;
    Sprite wallSprite;
    Texture wallTex;
    char** lvl;
    const int cell_size = 64;
    const int gridHeight = 14;
    const int gridWidth = 110;
	Clock gameClock;

public:
    GameManager() : window(VideoMode(1200, 900), "Sonic") {
        initialize();
    }

    void initialize() {
        // Load textures
        sonicTex.loadFromFile("sonic.png");
        wallTex.loadFromFile("brick1.png");
        wallSprite.setTexture(wallTex);

        // Initialize level grid
        lvl = new char* [gridHeight];
        for (int i = 0; i < gridHeight; i++) {
            lvl[i] = new char[gridWidth] {'\0'};
        }
        for (int i = 0; i < gridWidth; i++) {
            lvl[11][i] = 'w'; // Ground layer
        }

        // Initialize Sonic
        sonic.initialize(100.0f, 11 * cell_size - 64, sonicTex, 2.0f);
    }

    void run() {
        while (window.isOpen()) {
			float deltaTime = gameClock.restart().asMilliseconds();
            handleInput();
            update(deltaTime);
            render();
        }
    }

private:
    void handleInput() {
        Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == Event::Closed) window.close();
            if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::Space) {
                sonic.jump();
            }
        }

        // Horizontal movement
        if (Keyboard::isKeyPressed(Keyboard::Left)) {
            sonic.moveHorizontal(-1);
        }
        else if (Keyboard::isKeyPressed(Keyboard::Right)) {
            sonic.moveHorizontal(1);
        }
        else
		{
			sonic.moveHorizontal(0);
        }
        
        if (Keyboard::isKeyPressed(Keyboard::Space))
        {
            sonic.jump();
        }
    }

    void update(float deltaTime) {
        //float deltaTime = gameClock.restart().asMilliseconds();
		//float deltaTime = 2.0f / 60.0f; // Fixed time step for simplicity
        checkCollisions();
        sonic.update(deltaTime);
    }

    void checkCollisions() {
        Hitbox hb = sonic.getHitbox();
        bool wasGrounded = sonic.getisGrounded();
        sonic.setGrounded(false);

        // Ground collision
        int checkY = static_cast<int>((hb.y + hb.height) / cell_size);
        int leftX = static_cast<int>(hb.x / cell_size);
        int rightX = static_cast<int>((hb.x + hb.width) / cell_size);

        if (checkY < gridHeight && (lvl[checkY][leftX] == 'w' || lvl[checkY][rightX] == 'w')) {
            sonic.setY(checkY * cell_size - hb.height - 5);
            sonic.setGrounded(true);
            sonic.setSpeedY(0);
        }
        else {
            sonic.setGrounded(false);
        }

        // Wall collision
        int checkX = static_cast<int>(hb.x / cell_size);
        int headY = static_cast<int>(hb.y / cell_size);
        if (lvl[headY][checkX] == 'w') {
            sonic.setSpeedX(0);
        }
    }

    void render() {
        window.clear();

        // Draw level
        for (int i = 0; i < gridHeight; i++) {
            for (int j = 0; j < gridWidth; j++) {
                if (lvl[i][j] == 'w') {
                    wallSprite.setPosition(j * cell_size, i * cell_size);
                    window.draw(wallSprite);
                }
            }
        }

        // Draw Sonic
        Sprite s = sonic.getSprite();
        s.setPosition(sonic.getX(), sonic.getY());
        window.draw(s);

        window.display();
    }
};