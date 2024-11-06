#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>

#include "EntityManager.hpp"

class Game {
private:
    sf::RenderWindow m_window;
    sf::Clock m_clock;

    EntityManager m_entityManager;

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