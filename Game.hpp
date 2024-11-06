#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>

#include "EntityManager.hpp"

class Game {
private:
    sf::RenderWindow m_window;
    sf::Clock m_clock;

    EntityManager m_entityManager;

    std::shared_ptr<Entity> m_player;

    bool m_running = true;
    bool m_paused = false;

    unsigned int m_currentFrame = 0;
    unsigned int m_lastEnemySpawnTime = 0; 

    void setUp();

    void sUserInput();
    void sMovement();
    void sCollision();
    void sLifeSpan();
    void sRender();

    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallerEnemies(std::shared_ptr<Entity> entity);
    void enemySpawner();
    void spawnBullet(std::shared_ptr<Entity> entity, const sf::Vector2f& mousePos);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

    void setPaused(bool paused);
    void quit();

public:
    Game();
    void run();
};