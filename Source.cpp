#include "PlayerManager.h"

int screen_x = 1200;
int screen_y = 900;



// Existing prototypes (unchanged)
void display_level(RenderWindow&, const int, const int, char**, Sprite&, const int, float);

int main() {
    RenderWindow window(VideoMode(screen_x, screen_y), "Sonic the Hedgehog-OOP", Style::Close);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    // Level initialization (unchanged)
    const int cell_size = 64;
    const int height = 14;
    const int width = 110;

    char** lvl = new char* [height];
    for (int i = 0; i < height; i++) {
        lvl[i] = new char[width] {'\0'};
        fill_n(lvl[i], width, 's');
    }

    for (int j = 0; j < width; j++) {
        lvl[height - 1][j] = 'w';
        //if (j % 3 == 0) lvl[height - 4][j] = 'w';
    }

    Texture wallTex1;
    wallTex1.loadFromFile("Data/brick1.png");
    Sprite wallSprite1(wallTex1);

    Music lvlMus;
    lvlMus.openFromFile("Data/labrynth.ogg");
    lvlMus.setVolume(30);
    lvlMus.play();
    lvlMus.setLoop(true);

    // Create player instance
	PlayerManager player;

	Clock gameClock;
	float deltaTime = 0.0f;

    // Main loop (unchanged structure)
    Event ev;
    while (window.isOpen()) {

		deltaTime = gameClock.restart().asSeconds(); 

        while (window.pollEvent(ev)) 
        {
            if (ev.type == Event::Closed) window.close();
        }


        player.handleInput(lvl, cell_size);
        player.updatePhysics(lvl, cell_size);

        // Camera calculations (unchanged)
        float level_width = width * cell_size;
        float half_screen = screen_x / 2.0f;
        // Calculate desired camera position
        float desired_camera = player.getPlayerX() - half_screen;

        // Left boundary check
        if (desired_camera < 0) desired_camera = 0;

        // Right boundary check
        if (desired_camera > level_width - screen_x)
            desired_camera = level_width - screen_x;
        float camera_offset_x = 0; // Initialize camera offset
        camera_offset_x = desired_camera;

        // Rendering (unchanged structure)
        window.clear();
        display_level(window, height, width, lvl, wallSprite1, cell_size, desired_camera);
        player.draw(window, desired_camera);
        window.display();
    }

    // Cleanup (unchanged)
    for (int i = 0; i < height; i++) delete[] lvl[i];
    delete[] lvl;
    return 0;
}

// Existing functions (unchanged)
void display_level(RenderWindow& window, const int height, const int width, char** lvl, Sprite& wallSprite1, const int cell_size, float camera_offset_x) {
    int start_col = static_cast<int>(camera_offset_x / cell_size);
    int end_col = static_cast<int>((camera_offset_x + screen_x) / cell_size);
    start_col = max(start_col, 0);
    end_col = min(end_col, width - 1);

    for (int i = 0; i < height; i++) {
        for (int j = start_col; j <= end_col; j++) {
            if (lvl[i][j] == 'w') {
                wallSprite1.setPosition(j * cell_size - camera_offset_x, i * cell_size);
                window.draw(wallSprite1);
            }
        }
    }
}