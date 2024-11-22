#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>

#include "EntityManager.hpp"
#include "Random.hpp"

class Game {
private:
    sf::RenderWindow m_window;
    sf::Clock m_clock;
    sf::Font m_font;

    Random m_random;
    EntityManager m_entityManager;

    std::shared_ptr<Entity> m_player;
    std::shared_ptr<Entity> m_score;
    std::shared_ptr<Entity> m_highScore;
    std::shared_ptr<Entity> m_lives;
    std::shared_ptr<Entity> m_timer;

    bool m_running = true;
    bool m_paused = false;
    bool m_startTimer = true;
    bool m_brokeHighScore = true;

    int m_startTimerTotal = 3;
    int m_startTimerRemaining = m_startTimerTotal;

    unsigned int m_currentFrame = 0;
    unsigned int m_lastEnemySpawnTime = 0; 

    void setUp();

    void sUserInput();
    void sMovement();
    void sCollision();
    void sLifeSpan();
    void sAnimation();
    void sRender();
    void sTrail();

    void spawnNewHighScore();
    void spawnStartTimer();
    void spawnStar();
    void spawnScore();
    void spawnHighScore();
    void setHighScore();
    void spawnLives();
    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallerEnemies(std::shared_ptr<Entity> entity);
    void enemySpawner();
    void spawnBullet(std::shared_ptr<Entity> entity, const sf::Vector2f& target);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity);
    void spawnGlyph(std::shared_ptr<Entity> entity);
    void spawnParticles(std::shared_ptr<Entity> entity);

    void setPaused(bool paused);
    void quit();

public:
    Game();
    void run();
};