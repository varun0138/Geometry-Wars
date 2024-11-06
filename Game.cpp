#include "Game.hpp"
#include "Constants.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

Game::Game() {
    m_window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "", sf::Style::Close | sf::Style::Titlebar);
    m_window.setPosition(sf::Vector2i(5, 5));
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

        if(!m_paused) {
            enemySpawner();
            sMovement();
            sCollision();
            sLifeSpan();

            m_currentFrame++;
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
                break;

            case sf::Event::KeyPressed:
                if(event.key.code == sf::Keyboard::W)      { m_player->cInput->up   = true; }
                else if(event.key.code == sf::Keyboard::S) { m_player->cInput->down = true; }
                else if(event.key.code == sf::Keyboard::A) { m_player->cInput->left = true; }
                else if(event.key.code == sf::Keyboard::D) { m_player->cInput->right = true; }
                break;

            case sf::Event::KeyReleased:
                if(event.key.code == sf::Keyboard::W)      { m_player->cInput->up   = false; }
                else if(event.key.code == sf::Keyboard::S) { m_player->cInput->down = false; }
                else if(event.key.code == sf::Keyboard::A) { m_player->cInput->left = false; }
                else if(event.key.code == sf::Keyboard::D) { m_player->cInput->right = false; }
                break;

            default:
                break;
        }
    }
}

void Game::sMovement() {
    m_player->cTransform->velocity = { 0.0f, 0.0f };

    if(m_player->cInput->up)    { m_player->cTransform->velocity.y = -1 * PLAYER_SPEED; }
    if(m_player->cInput->down)  { m_player->cTransform->velocity.y = PLAYER_SPEED; }
    if(m_player->cInput->left)  { m_player->cTransform->velocity.x = -1 * PLAYER_SPEED; }
    if(m_player->cInput->right) { m_player->cTransform->velocity.x = PLAYER_SPEED; }

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
    const float radius = m_random.randint(ENEMY_MIN_RADIUS, ENEMY_MAX_RADIUS);
    const unsigned int vertices = m_random.randint(ENEMY_MIN_VERTICES, ENEMY_MAX_VERTICES);
    const sf::Color color = { (sf::Uint8)m_random.randint(100, 250), (sf::Uint8)m_random.randint(100, 250), (sf::Uint8)m_random.randint(100, 250) };

    const sf::FloatRect bounds = { m_player->cTransform->pos.x - PLAYER_RADIUS * 6, m_player->cTransform->pos.y - PLAYER_RADIUS * 6, PLAYER_RADIUS * 12, PLAYER_RADIUS * 12 };

    sf::Vector2f pos = { m_random.randrange(radius, WINDOW_WIDTH - radius, radius * 4), m_random.randrange(radius, WINDOW_HEIGHT - radius, radius * 4) };
    while(bounds.contains(pos)) {
        pos = { m_random.randrange(radius, WINDOW_WIDTH - radius, radius * 4), m_random.randrange(radius, WINDOW_HEIGHT - radius, radius * 4) };
    }

    const sf::Vector2f velocity = { m_random.randfloat(ENEMY_MIN_SPEED, ENEMY_MAX_SPEED),  m_random.randfloat(ENEMY_MIN_SPEED, ENEMY_MAX_SPEED) };

    auto enemy = m_entityManager.addEntity("Enemy");
    enemy->cTransform = std::make_shared<CTransform>(pos, velocity, 0.0f);
    enemy->cShape = std::make_shared<CShape>(radius, vertices, color, ENEMY_OUTLINE_COLOR, ENEMY_OUTLINE_THICKNESS);
    enemy->cCollision = std::make_shared<CCollision>(radius);
}

void Game::spawnSmallerEnemies(std::shared_ptr<Entity> entity) {

}

void Game::enemySpawner() {
    if(m_currentFrame - m_lastEnemySpawnTime == ENEMY_SPAWN_INTERVAL) {
        spawnEnemy();
        m_lastEnemySpawnTime = m_currentFrame;
    }
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