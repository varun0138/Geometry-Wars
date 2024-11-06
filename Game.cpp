#include "Game.hpp"

#include <SFML/Window/Event.hpp>

Game::Game() {
    m_window.create(sf::VideoMode(1980, 1080), "", sf::Style::None);
}

void Game::run() {
    while(m_running) {

        sUserInput();

        if(m_paused) {
            sMovement();
        }
        
        sRender();
    }
}

void Game::sUserInput() {
    sf::Event event;
    while(m_window.pollEvent(event)) {
        switch(event.type) {
            case sf::Event::Closed:
                quit();

            case sf::Event::KeyPressed:
                if(event.key.code == sf::Keyboard::Escape) {
                    quit();
                }
                break;

            default:
                break;
        }
    }
}

void Game::sMovement() {

}

void Game::sRender() {
    m_window.clear();


    m_window.display();
}

void Game::quit() {
    m_running = false;
    m_window.close();
}