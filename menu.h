#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
using namespace std;
using namespace sf;

class IntroAnimation {
private:
    RenderWindow& window;
    Texture segaTexture;
    Sprite segaSprite;
    SoundBuffer segaBuffer;
    Sound segaSound;
    IntRect frames[21];
    int currentFrame;
    Clock clock;
    bool soundPlayed;

public:
    IntroAnimation(RenderWindow& win) : window(win), currentFrame(0), soundPlayed(false) {
        if (!segaTexture.loadFromFile("Data/sega-logo.png")) {
            cout << "Error loading sega-logo.png\n";
        }

        int frameHeight = segaTexture.getSize().y / 21;
        for (int i = 0; i < 21; ++i) {
            frames[i] = IntRect(0, i * frameHeight, segaTexture.getSize().x, frameHeight);
        }

        segaSprite.setTexture(segaTexture);
        segaSprite.setScale(3.5f, 3.5f);
        segaSprite.setPosition(50, 300);

        if (!segaBuffer.loadFromFile("Data/Sega.wav")) {
            cout << "Error loading Sega.wav\n";
        }
        segaSound.setBuffer(segaBuffer);
    }

    bool run() {
        const float frameDuration = 0.065f;
        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    window.close();
                    return false;
                }
            }

            if (!soundPlayed && currentFrame == 2) {
                segaSound.play();
                soundPlayed = true;
            }

            if (clock.getElapsedTime().asSeconds() > frameDuration && currentFrame < 20) {
                currentFrame++;
                clock.restart();
            }

            segaSprite.setTextureRect(frames[currentFrame]);

            window.clear(Color::White);
            window.draw(segaSprite);
            window.display();

            if (currentFrame >= 20) {
                sleep(seconds(frameDuration));
                return true;
            }
        }
        return false;
    }
};

class Game {
private:
    RenderWindow& window;
    Font font1, font2, font3, font4;
    Text sonic, start_game, help, high_scores, settings, quit, main_text;
    Text difficulty, normal_mode, boss_level, back_to_menu;
    Text title, music, music_state, music_vol, music_volume, sound_eff, sound_eff_state, sound_eff_vol, sound_e_volume, settings_text;
    Text highscore;
    Text level1, level2, level3, levelBack;
    Texture backgroundTexture, help_manual_texture;
    Sprite backgroundSprite, help_manual;
    Music bgMusic;
    SoundBuffer menuButtonBuffer;
    Sound menu_button;

    const int MAIN_MENU = 0;
    const int GAME_MODE_SELECTION = 1;
    const int SETTINGS = 2;
    const int HIGH_SCORES = 3;
    const int LEVEL_SELECTION = 4;
    int currentScreen;
    int main_option;
    int game_mode;
    int settings_option;
    bool music_on;
    int tmusic_vol;
    bool sound_e_on;
    int sound_e_vol;
    int levelOption;
    int selectedLevel = 0; // 0 means quit, 1/2/3 for levels

public:
    Game(RenderWindow& win) : window(win),
        currentScreen(MAIN_MENU), main_option(1), game_mode(1),levelOption(1), settings_option(1),
        music_on(true), tmusic_vol(50), sound_e_on(true), sound_e_vol(80) {

        // Load SEGA font for title
        if (!font1.loadFromFile("Data/Sega.TTF")) {
            cout << "Error loading Sega.TTF font" << endl;
        }

        // Load Johnny Fever font for menu options
        if (!font2.loadFromFile("Data/Johnny Fever.otf")) {
            cout << "Error loading Johnny Fever.otf font" << endl;
        }

        if (!font3.loadFromFile("Data/Eating Pasta.ttf")) {
            cout << "Error loading font3" << endl;
        }

        if (!font4.loadFromFile("Data/HurmeGeometricSans1.otf")) {
            cout << "Error loading font4" << endl;
        }

        // Initialize main menu title with SEGA font - large and centered
        sonic.setFont(font1);
        sonic.setString("SONIC CLASSIC HEROES");
        sonic.setCharacterSize(96);
        sonic.setFillColor(Color(0, 80, 200));
        sonic.setOutlineColor(Color(255, 200, 0));
        sonic.setOutlineThickness(5);
        // Center the title (window width 1200)
        sonic.setPosition(600 - sonic.getGlobalBounds().width / 2, 60);

        // Menu options with Frileder font
        start_game.setFont(font2);
        start_game.setString("Start Game");
        start_game.setCharacterSize(54);
        start_game.setFillColor(Color::White);
        start_game.setPosition(600 - start_game.getGlobalBounds().width / 2, 280);

        help.setFont(font2);
        help.setString("Help");
        help.setCharacterSize(54);
        help.setFillColor(Color::White);
        help.setPosition(600 - help.getGlobalBounds().width / 2, 360);

        high_scores.setFont(font2);
        high_scores.setString("High Scores");
        high_scores.setCharacterSize(54);
        high_scores.setFillColor(Color::White);
        high_scores.setPosition(600 - high_scores.getGlobalBounds().width / 2, 440);

        settings.setFont(font2);
        settings.setString("Settings");
        settings.setCharacterSize(54);
        settings.setFillColor(Color::White);
        settings.setPosition(600 - settings.getGlobalBounds().width / 2, 520);

        quit.setFont(font2);
        quit.setString("Quit Game");
        quit.setCharacterSize(54);
        quit.setFillColor(Color::White);
        quit.setPosition(600 - quit.getGlobalBounds().width / 2, 600);

        main_text.setFont(font2);
        main_text.setCharacterSize(54);
        main_text.setFillColor(Color::Yellow);  // Contrasts well with blue background
        main_text.setOutlineColor(Color::Black);
        main_text.setOutlineThickness(2);

        // Game mode selection texts
        difficulty.setFont(font1);
        difficulty.setString("SELECT MODE");
        difficulty.setCharacterSize(64);
        difficulty.setFillColor(Color(0, 80, 200));
        difficulty.setOutlineColor(Color(255, 200, 0));
        difficulty.setOutlineThickness(3);

        normal_mode.setFont(font2);
        normal_mode.setString("Normal Mode");
        normal_mode.setCharacterSize(48);
        normal_mode.setFillColor(Color::White);

        boss_level.setFont(font2);
        boss_level.setString("BOSS Level");
        boss_level.setCharacterSize(48);
        boss_level.setFillColor(Color::White);

        back_to_menu.setFont(font2);
        back_to_menu.setString("Back");
        back_to_menu.setCharacterSize(40);
        back_to_menu.setFillColor(Color(200, 200, 200));

        // Settings/title texts - use SEGA font for headings
        title.setFont(font1);
        title.setCharacterSize(64);
        title.setFillColor(Color(0, 80, 200));
        title.setOutlineColor(Color(255, 200, 0));
        title.setOutlineThickness(3);

        level1.setFont(font2);
        level1.setString("Level 1: Labyrinth Zone");
        level1.setCharacterSize(44);
        level1.setFillColor(Color::White);

        level2.setFont(font2);
        level2.setString("Level 2: Ice Cap Zone");
        level2.setCharacterSize(44);
        level2.setFillColor(Color::White);

        level3.setFont(font2);
        level3.setString("Level 3: Death Egg Zone");
        level3.setCharacterSize(44);
        level3.setFillColor(Color::White);

        levelBack.setFont(font2);
        levelBack.setString("Back");
        levelBack.setCharacterSize(40);
        levelBack.setFillColor(Color(200, 200, 200));
        levelBack.setPosition(50, 800);

        music.setFont(font2);
        music.setString("Music:");
        music.setCharacterSize(42);
        music.setFillColor(Color::White);
        music.setPosition(200, 220);

        music_state.setFont(font2);
        music_state.setCharacterSize(42);
        music_state.setFillColor(Color::White);
        music_state.setPosition(420, 220);

        music_vol.setFont(font2);
        music_vol.setString("Music Volume:");
        music_vol.setCharacterSize(42);
        music_vol.setFillColor(Color::White);
        music_vol.setPosition(200, 300);

        music_volume.setFont(font2);
        music_volume.setCharacterSize(42);
        music_volume.setFillColor(Color::White);
        music_volume.setPosition(540, 300);

        sound_eff.setFont(font2);
        sound_eff.setString("Sound Effects:");
        sound_eff.setCharacterSize(42);
        sound_eff.setFillColor(Color::White);
        sound_eff.setPosition(200, 420);

        sound_eff_state.setFont(font2);
        sound_eff_state.setCharacterSize(42);
        sound_eff_state.setFillColor(Color::White);
        sound_eff_state.setPosition(540, 420);

        sound_eff_vol.setFont(font2);
        sound_eff_vol.setString("Effects Volume:");
        sound_eff_vol.setCharacterSize(42);
        sound_eff_vol.setFillColor(Color::White);
        sound_eff_vol.setPosition(200, 500);

        sound_e_volume.setFont(font2);
        sound_e_volume.setCharacterSize(42);
        sound_e_volume.setFillColor(Color::White);
        sound_e_volume.setPosition(540, 500);

        settings_text.setFont(font2);
        settings_text.setCharacterSize(42);
        settings_text.setFillColor(Color::Yellow);  // Contrasts well with blue background
        settings_text.setOutlineColor(Color::Black);
        settings_text.setOutlineThickness(2);

        // High scores
        highscore.setFont(font2);
        highscore.setString("High Scores Placeholder");
        highscore.setCharacterSize(44);
        highscore.setFillColor(Color::White);

        // Load textures - using title screen as menu background
        if (!backgroundTexture.loadFromFile("Data/title_screen.gif")) {
            cout << "Error loading background texture" << endl;
        }
        if (!help_manual_texture.loadFromFile("Data/help-manual.png")) {
            cout << "Error loading help manual texture" << endl;
        }
        backgroundSprite.setTexture(backgroundTexture);
        // Reduce background opacity by 25% (75% visible = 191 alpha)
        backgroundSprite.setColor(Color(255, 255, 255, 191));
        // Scale background to fit window (1200x900)
        float scaleX = 1200.0f / backgroundTexture.getSize().x;
        float scaleY = 900.0f / backgroundTexture.getSize().y;
        float scale = max(scaleX, scaleY);  // Use larger scale to cover entire screen
        backgroundSprite.setScale(scale, scale);
        // Center the background
        backgroundSprite.setPosition(
            (1200.0f - backgroundTexture.getSize().x * scale) / 2.0f,
            (900.0f - backgroundTexture.getSize().y * scale) / 2.0f
        );
        help_manual.setTexture(help_manual_texture);
        help_manual.setScale(0.5f, 0.5f);
        help_manual.setPosition(15, 160);

        // Audio
        if (!bgMusic.openFromFile("Data/risk.ogg")) {
            cout << "Error loading background music" << endl;
        }
        bgMusic.setVolume(tmusic_vol);
        bgMusic.setLoop(true);

        if (!menuButtonBuffer.loadFromFile("Data/menubutton.wav")) {
            cout << "Error loading menu button sound" << endl;
        }
        menu_button.setBuffer(menuButtonBuffer);
        menu_button.setVolume(sound_e_vol);
    }

    int run() {
        if (music_on) bgMusic.play();
        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();
                else if (event.type == Event::KeyReleased)
                    handleInput(event.key.code);
            }
            update();
            render();
            // If a level is selected, close window and return
            if (selectedLevel > 0) {
                window.close();
                return selectedLevel;
            }
        }
        return 0; // 0 means quit
    }

private:
    void handleInput(Keyboard::Key key) {
        if (currentScreen == MAIN_MENU) {
            handleMainMenuInput(key);
        }
        else if (currentScreen == GAME_MODE_SELECTION) {
            handleGameModeInput(key);
        }
		else if (currentScreen == LEVEL_SELECTION) {
			handleLevelSelectionInput(key);
		}
        else if (currentScreen == SETTINGS) {
            handleSettingsInput(key);
        }
        else if (currentScreen == HIGH_SCORES) {
            handleHighScoresInput(key);
        }
    }

    void handleMainMenuInput(Keyboard::Key key) {
        if (key == Keyboard::Up) {
            if (sound_e_on) {
                menu_button.play();
            }
            main_option = (main_option == 1) ? 5 : main_option - 1;
        }
        else if (key == Keyboard::Down) {
            if (sound_e_on) {
                menu_button.play();
            }
            main_option = (main_option == 5) ? 1 : main_option + 1;
        }
        else if (key == Keyboard::Enter) {
            switch (main_option) {
            case 1: // Start Game - go directly to level selection
                currentScreen = LEVEL_SELECTION;
                levelOption = 1;
                break;
            case 2: // Help
                break;
            case 3: // High Scores
                currentScreen = HIGH_SCORES;
                break;
            case 4: // Settings
                currentScreen = SETTINGS;
                settings_option = 1;
                break;
            case 5: // Quit Game
                window.close();
                break;
            }
        }
    }

    void handleGameModeInput(Keyboard::Key key) {
        if (key == Keyboard::Right || key == Keyboard::Down) {
            if (sound_e_on) {
                menu_button.play();
            }
            game_mode = (game_mode == 1) ? 2 : 1;
        }
        else if (key == Keyboard::Left || key == Keyboard::Up) {
            if (sound_e_on) {
                menu_button.play();
            }
            game_mode = (game_mode == 1) ? 2 : 1;
        }
        else if (key == Keyboard::Escape) {
            currentScreen = MAIN_MENU;
            main_option = 1;
        }
        else if (key == Keyboard::Enter) {
            if (game_mode == 1) { // Normal Mode selected
                currentScreen = LEVEL_SELECTION;
                levelOption = 1;
            }
            else { // Boss Level
                cout << "Starting BOSS Level" << endl;
                // Start boss level logic
            }
        }
    }

    void handleLevelSelectionInput(Keyboard::Key key) {
        if (key == Keyboard::Right || key == Keyboard::Down) {
            if (sound_e_on) menu_button.play();
            levelOption = (levelOption == 3) ? 1 : levelOption + 1;
        }
        else if (key == Keyboard::Left || key == Keyboard::Up) {
            if (sound_e_on) menu_button.play();
            levelOption = (levelOption == 1) ? 3 : levelOption - 1;
        }
        else if (key == Keyboard::Escape) {
            currentScreen = MAIN_MENU;
            main_option = 1;
        }
        else if (key == Keyboard::Enter) {
            selectedLevel = levelOption; // Set selected level
        }
    }

    void handleSettingsInput(Keyboard::Key key) {
        if (key == Keyboard::Up) {
            if (sound_e_on) {
                menu_button.play();
            }
            settings_option = (settings_option == 1) ? 4 : settings_option - 1;
        }
        else if (key == Keyboard::Down) {
            if (sound_e_on) {
                menu_button.play();
            }
            settings_option = (settings_option == 4) ? 1 : settings_option + 1;
        }
        else if (key == Keyboard::Right || key == Keyboard::Left) {
            if (sound_e_on) {
                menu_button.play();
            }
            switch (settings_option) {
            case 1: // Music On/Off
                music_on = !music_on;
                if (music_on) {
                    bgMusic.play();
                }
                else {
                    bgMusic.stop();
                }
                break;
            case 2: // Music Volume
                if (key == Keyboard::Right && tmusic_vol < 100) {
                    tmusic_vol++;
                }
                else if (key == Keyboard::Left && tmusic_vol > 0) {
                    tmusic_vol--;
                }
                bgMusic.setVolume(tmusic_vol);
                break;
            case 3: // Sound Effects On/Off
                sound_e_on = !sound_e_on;
                break;
            case 4: // Sound Effects Volume
                if (key == Keyboard::Right && sound_e_vol < 100) {
                    sound_e_vol++;
                }
                else if (key == Keyboard::Left && sound_e_vol > 0) {
                    sound_e_vol--;
                }
                menu_button.setVolume(sound_e_vol);
                break;
            }
        }
        else if (key == Keyboard::Escape) {
            currentScreen = MAIN_MENU;
            main_option = 1;
        }
    }

    void handleHighScoresInput(Keyboard::Key key) {
        if (key == Keyboard::Escape || key == Keyboard::Enter) {
            currentScreen = MAIN_MENU;
            main_option = 1;
        }
    }

    void update() {
        if (currentScreen == MAIN_MENU) {
            updateMainMenu();
        }
        else if (currentScreen == LEVEL_SELECTION) {
            // Selected level: yellow with black outline, unselected: white
            level1.setFillColor(levelOption == 1 ? Color::Yellow : Color::White);
            level1.setOutlineColor(Color::Black);
            level1.setOutlineThickness(levelOption == 1 ? 2.0f : 0.0f);
            
            level2.setFillColor(levelOption == 2 ? Color::Yellow : Color::White);
            level2.setOutlineColor(Color::Black);
            level2.setOutlineThickness(levelOption == 2 ? 2.0f : 0.0f);
            
            level3.setFillColor(levelOption == 3 ? Color::Yellow : Color::White);
            level3.setOutlineColor(Color::Black);
            level3.setOutlineThickness(levelOption == 3 ? 2.0f : 0.0f);
        }
        else if (currentScreen == GAME_MODE_SELECTION) {
            updateGameMode();
        }
        else if (currentScreen == SETTINGS) {
            updateSettings();
        }
        else if (currentScreen == HIGH_SCORES) {
            // No updates needed
        }
    }

    void updateMainMenu() {
        switch (main_option) {
        case 1:
            main_text.setString("Start Game");
            main_text.setPosition(600 - main_text.getGlobalBounds().width / 2, 280);
            break;
        case 2:
            main_text.setString("Help");
            main_text.setPosition(600 - main_text.getGlobalBounds().width / 2, 360);
            break;
        case 3:
            main_text.setString("High Scores");
            main_text.setPosition(600 - main_text.getGlobalBounds().width / 2, 440);
            break;
        case 4:
            main_text.setString("Settings");
            main_text.setPosition(600 - main_text.getGlobalBounds().width / 2, 520);
            break;
        case 5:
            main_text.setString("Quit Game");
            main_text.setPosition(600 - main_text.getGlobalBounds().width / 2, 600);
            break;
        }
    }

    void updateGameMode() {
        if (game_mode == 1) {
            main_text.setString("Normal Mode");
            main_text.setPosition(250, 400);
        }
        else {
            main_text.setString("BOSS Level");
            main_text.setPosition(700, 400);
        }
    }

    void updateSettings() {
        music_state.setString(music_on ? "ON" : "OFF");
        music_volume.setString(to_string(tmusic_vol));
        sound_eff_state.setString(sound_e_on ? "ON" : "OFF");
        sound_e_volume.setString(to_string(sound_e_vol));

        switch (settings_option) {
        case 1:
            settings_text.setString(music_on ? "ON" : "OFF");
            settings_text.setPosition(420, 220);
            break;
        case 2:
            settings_text.setString(to_string(tmusic_vol));
            settings_text.setPosition(540, 300);
            break;
        case 3:
            settings_text.setString(sound_e_on ? "ON" : "OFF");
            settings_text.setPosition(540, 420);
            break;
        case 4:
            settings_text.setString(to_string(sound_e_vol));
            settings_text.setPosition(540, 500);
            break;
        }
    }

    void render() {
        window.clear();
        window.draw(backgroundSprite);

        if (currentScreen == MAIN_MENU) {
            window.draw(sonic);
            window.draw(start_game);
            window.draw(help);
            window.draw(high_scores);
            window.draw(settings);
            window.draw(quit);
            window.draw(main_text);
        }
        else if (currentScreen == GAME_MODE_SELECTION) {
            // Center the difficulty title
            difficulty.setPosition(600 - difficulty.getGlobalBounds().width / 2, 100);
            // Position mode options
            normal_mode.setPosition(250, 400);
            boss_level.setPosition(700, 400);
            back_to_menu.setPosition(50, 800);
            window.draw(difficulty);
            window.draw(normal_mode);
            window.draw(boss_level);
            window.draw(main_text);
            window.draw(back_to_menu);
        }
        else if (currentScreen == LEVEL_SELECTION) {
            title.setString("SELECT LEVEL");
            title.setPosition(600 - title.getGlobalBounds().width / 2, 80);
            // Center level options
            level1.setPosition(600 - level1.getGlobalBounds().width / 2, 280);
            level2.setPosition(600 - level2.getGlobalBounds().width / 2, 380);
            level3.setPosition(600 - level3.getGlobalBounds().width / 2, 480);
            window.draw(title);
            window.draw(level1);
            window.draw(level2);
            window.draw(level3);
            window.draw(levelBack);
        }
        else if (currentScreen == SETTINGS) {
            back_to_menu.setPosition(50, 800);
            title.setString("SETTINGS");
            title.setPosition(600 - title.getGlobalBounds().width / 2, 80);
            window.draw(title);
            window.draw(music);
            window.draw(music_state);
            window.draw(music_vol);
            window.draw(music_volume);
            window.draw(sound_eff);
            window.draw(sound_eff_state);
            window.draw(sound_eff_vol);
            window.draw(sound_e_volume);
            window.draw(back_to_menu);
            window.draw(settings_text);
        }
        else if (currentScreen == HIGH_SCORES) {
            back_to_menu.setPosition(50, 600);
            title.setString("HIGH SCORES");
            title.setPosition(600 - title.getGlobalBounds().width / 2, 80);
            highscore.setPosition(600 - highscore.getGlobalBounds().width / 2, 300);
            window.draw(title);
            window.draw(highscore);
            window.draw(back_to_menu);
        }

        window.display();
    }
};

inline int showMenu(sf::RenderWindow& window) {
    IntroAnimation intro(window);
    if (!intro.run()) return 0;
    Game game(window);
    return game.run();
} 