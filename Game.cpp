#include "Game.hpp"
#include "Constants.hpp"

#include <SFML/Window/Event.hpp>

Game::Game() {
    m_window.create(sf::VideoMode(1980, 1080), "", sf::Style::None);
    m_window.setFramerateLimit(60);

    setUp();
}

void Game::setUp() {
    spawnPlayer();
}

void Game::run() {
    while(m_running) {
        
        m_entityManager.update();
        sUserInput();

        if(m_paused) {
            sMovement();
            sCollision();
            sLifeSpan();
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

void Game::sCollision() {

}

void Game::sLifeSpan() {

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

void Game::spawnPlayer() {
    m_player = m_entityManager.addEntity("Player");
    m_player->cTransform = std::make_shared<CTransform>((sf::Vector2f)m_window.getSize() / 2.0f, sf::Vector2f(PLAYER_SPEED, PLAYER_SPEED), 0.0f);
    m_player->cShape = std::make_shared<CShape>(PLAYER_RADIUS, PLAYER_VERTICES, PLAYER_FILL_COLOR, PLAYER_OUTLINE_COLOR, PLAYER_OUTLINE_THICKNESS);
    m_player->cCollision = std::make_shared<CCollision>(PLAYER_RADIUS);
    m_player->cInput = std::make_shared<CInput>();
    m_player->cScore = std::make_shared<CScore>();
}

void Game::spawnEnemy() {

}

void Game::spawnSmallerEnemies(std::shared_ptr<Entity> entity) {

}

void Game::enemySpawner() {

}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const sf::Vector2f& mousePos) {

}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {

}

void Game::setPaused(bool paused) {
    m_paused = !m_paused;
}

void Game::quit() {
    m_running = false;
    m_window.close();
}