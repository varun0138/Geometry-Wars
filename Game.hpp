#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>

class Game {
private:
    sf::RenderWindow m_window;
    sf::Clock m_clock;

    bool m_running = true;
    bool m_paused = false;

    void sUserInput();
    void sMovement();
    void sRender();

    void quit();

public:
    Game();
    void run();
};