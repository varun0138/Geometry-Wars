#include "Game.hpp"

#include <SFML/Window/Event.hpp>

Game::Game() {
    m_window.create(sf::VideoMode(1980, 1080), "", sf::Style::None);
}

void Game::run() {
    while(m_running) {
        
        m_entityManager.update();
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
    for(auto& entity: m_entityManager.getEntities()) {
        entity->cTransform->pos += entity->cTransform->velocity;
    }
}

void Game::sRender() {
    m_window.clear();

    for(auto& entity: m_entityManager.getEntities()) {
        entity->cShape->circle.setPosition(entity->cTransform->pos);

        entity->cTransform->angle += 1.0f;
        entity->cShape->circle.setRotation(entity->cTransform->angle);

        m_window.draw(entity->cShape->circle);
    }

    m_window.display();
}

void Game::quit() {
    m_running = false;
    m_window.close();
}