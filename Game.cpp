#include <SFML/Graphics.hpp>
#include <iostream>
#include "menu.h"
#include "GameManager.h"

using namespace sf;

int main()
{
    RenderWindow window(VideoMode(1200, 900), "Sonic Game");
    int selectedLevel = showMenu(window);
    if (selectedLevel > 0) {
        GameManager game(selectedLevel);
        game.run();
    }
    return 0;
}