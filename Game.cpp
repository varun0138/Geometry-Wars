#include "Game.hpp"
#include "Constants.hpp"

#include <SFML/Window/Event.hpp>
#include <cmath>
#include <iostream>

Game::Game() {
    m_window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "", sf::Style::Close | sf::Style::Titlebar);
    m_window.setPosition(sf::Vector2i(100, 5));
    m_window.setFramerateLimit(60);

    if(!m_font.loadFromFile("./fonts/Retron2000.ttf")) {
        std::cerr << "Error loading font file!!" << std::endl;
    }

    setUp();
}

void Game::setUp() {
    spawnPlayer();
    spawnScore();
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
            sAnimation();

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
                else if(event.key.code == sf::Keyboard::P) { setPaused(!m_paused); }
                break;

            case sf::Event::KeyReleased:
                if(event.key.code == sf::Keyboard::W)      { m_player->cInput->up   = false; }
                else if(event.key.code == sf::Keyboard::S) { m_player->cInput->down = false; }
                else if(event.key.code == sf::Keyboard::A) { m_player->cInput->left = false; }
                else if(event.key.code == sf::Keyboard::D) { m_player->cInput->right = false; }
                break;

            case sf::Event::MouseButtonPressed:
                if(event.mouseButton.button == sf::Mouse::Left) {
                    m_player->cInput->shoot = true;
                }
                else if(event.mouseButton.button == sf::Mouse::Right) {
                    spawnSpecialWeapon(m_player);
                }
                break;

            default:
                break;
        }
    }
}

void Game::sMovement() {
    m_player->cTransform->velocity = { 0.0f, 0.0f };

    if(m_player->cInput->up)    { m_player->cTransform->velocity.y = -1 * PLAYER_SPEED; spawnParticles(m_player);}
    if(m_player->cInput->down)  { m_player->cTransform->velocity.y = PLAYER_SPEED; spawnParticles(m_player); }
    if(m_player->cInput->left)  { m_player->cTransform->velocity.x = -1 * PLAYER_SPEED; spawnParticles(m_player); }
    if(m_player->cInput->right) { m_player->cTransform->velocity.x = PLAYER_SPEED; spawnParticles(m_player); }

    if(m_player->cInput->shoot) {
        sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(m_window);
        spawnBullet(m_player, mousePos);
        m_player->cInput->shoot = false;
    }

    for(auto& enemy: m_entityManager.getEntities("Enemy")) {
        if(enemy->cType) {
            if(enemy->cType->type == "Tracker") {
                sf::Vector2f velocity = m_player->cTransform->pos - enemy->cTransform->pos;
                float mag = std::sqrt(std::pow(velocity.x, 2) + std::pow(velocity.y, 2));
                velocity = { velocity.x / mag, velocity.y / mag };
                velocity *= ENEMY_MAX_SPEED;
                enemy->cTransform->velocity = velocity;
            }
        }
    }

    for(auto& entity: m_entityManager.getEntities()) {
        if(entity->cTransform) {
            entity->cTransform->pos += entity->cTransform->velocity;
        }
    }
}

void Game::sCollision() {

    // PLAYER BOUNDARY
    if(m_player->cTransform->pos.x <= m_player->cShape->radius) { m_player->cTransform->pos.x = m_player->cShape->radius; }
    if(m_player->cTransform->pos.x >= WINDOW_WIDTH - m_player->cShape->radius) { m_player->cTransform->pos.x = WINDOW_WIDTH - m_player->cShape->radius; } 
    if(m_player->cTransform->pos.y <= m_player->cShape->radius) { m_player->cTransform->pos.y = m_player->cShape->radius; }
    if(m_player->cTransform->pos.y >= WINDOW_HEIGHT - m_player->cShape->radius) { m_player->cTransform->pos.y = WINDOW_HEIGHT - m_player->cShape->radius; }

    // PLAYER ENEMY
    for(auto& enemy: m_entityManager.getEntities("Enemy")) {
        float radii = m_player->cShape->radius + enemy->cShape->radius;
        float distance = std::sqrt(std::pow(m_player->cTransform->pos.x - enemy->cTransform->pos.x, 2) + std::pow(m_player->cTransform->pos.y - enemy->cTransform->pos.y, 2));
        if(distance <= radii) {
            enemy->destroy();
            quit();
        }
    }

    // BULLET ENEMY
    for(auto& enemy: m_entityManager.getEntities("Enemy")) {
        for(auto& bullet: m_entityManager.getEntities("Bullet")) {
            float radii = bullet->cShape->radius+ enemy->cShape->radius;
            float distance = std::sqrt(std::pow(bullet->cTransform->pos.x - enemy->cTransform->pos.x, 2) + std::pow(bullet->cTransform->pos.y - enemy->cTransform->pos.y, 2));
            if(distance <= radii) {
                m_score->cScore->score += enemy->cShape->circle.getPointCount();
                m_score->cGlyph->text.setString("SCORE: " + std::to_string(m_score->cScore->score));
                spawnSmallerEnemies(enemy);
                spawnGlyph(enemy);
                enemy->destroy();
                bullet->destroy();
            }
        }
    }
}

void Game::sLifeSpan() {
    for(auto entity: m_entityManager.getEntities()) {
        if(entity->cLifeSpan) {
            float ratio = ((float)entity->cLifeSpan->remaining / (float)entity->cLifeSpan->total);

            if(entity->cLifeSpan->remaining <= 0 || ratio < 0.4) {
                entity->destroy();
                continue;
            }

            entity->cLifeSpan->remaining--;

            if(entity->cShape) {
                sf::Color fillColor = entity->cShape->circle.getFillColor();
                fillColor.a = 255 * ratio;
                entity->cShape->circle.setFillColor(fillColor);

                sf::Color outlineColor = entity->cShape->circle.getFillColor();
                outlineColor.a = 255 * ratio;
                entity->cShape->circle.setOutlineColor(outlineColor);
            }
            
            if(entity->cGlyph) {
                sf::Color fillColor = entity->cGlyph->text.getFillColor();
                fillColor.a = 255 * ratio;
                entity->cGlyph->text.setFillColor(fillColor);
            }
            
        }
    }
}

void Game::sAnimation() {
    const float speed = 0.035f;
    for(auto& entity: m_entityManager.getEntities("Enemy")) {
        sf::Vector2f scale = { 1.0f + 0.05f * std::sin(speed * m_currentFrame), 1.0f + 0.05f * std::cos(speed * m_currentFrame) };

        entity->cShape->circle.setScale(scale);
    }
}

void Game::sRender() {
    m_window.clear();

    for(auto& entity: m_entityManager.getEntities("Particle")) {
        if(entity->cTransform && entity->cShape) {
            entity->cShape->circle.setPosition(entity->cTransform->pos);

            entity->cTransform->angle += 1.0f;
            entity->cShape->circle.setRotation(entity->cTransform->angle);

            m_window.draw(entity->cShape->circle);
        }
    }

    for(auto& entity: m_entityManager.getEntities("Enemy")) {
        if(entity->cTransform && entity->cShape) {
            entity->cShape->circle.setPosition(entity->cTransform->pos);

            entity->cTransform->angle += 1.0f;
            entity->cShape->circle.setRotation(entity->cTransform->angle);

            m_window.draw(entity->cShape->circle);
        }
    }

    for(auto& entity: m_entityManager.getEntities("Small Enemy")) {
        if(entity->cTransform && entity->cShape) {
            entity->cShape->circle.setPosition(entity->cTransform->pos);

            entity->cTransform->angle += 1.0f;
            entity->cShape->circle.setRotation(entity->cTransform->angle);

            m_window.draw(entity->cShape->circle);
        }
    }

    for(auto& entity: m_entityManager.getEntities("Player")) {
        if(entity->cTransform && entity->cShape) {
            entity->cShape->circle.setPosition(entity->cTransform->pos);

            entity->cTransform->angle += 1.0f;
            entity->cShape->circle.setRotation(entity->cTransform->angle);

            m_window.draw(entity->cShape->circle);
        }
    }

    for(auto& entity: m_entityManager.getEntities("Bullet")) {
        if(entity->cTransform && entity->cShape) {
            entity->cShape->circle.setPosition(entity->cTransform->pos);

            entity->cTransform->angle += 1.0f;
            entity->cShape->circle.setRotation(entity->cTransform->angle);

            m_window.draw(entity->cShape->circle);
        }
    }

    for(auto& entity: m_entityManager.getEntities("Glyph")) {
        if(entity->cGlyph) {
            entity->cGlyph->text.setPosition(entity->cTransform->pos);

            m_window.draw(entity->cGlyph->text);
        }
    }

    m_window.display();
}

void Game::spawnScore() {
    m_score = m_entityManager.addEntity("Glyph"); 

    m_score->cTransform = std::make_shared<CTransform>(sf::Vector2f(5.0f, 5.0f), sf::Vector2f(0.0f, 0.0f), 0.0f);
    m_score->cScore = std::make_shared<CScore>(0);
    m_score->cGlyph = std::make_shared<CGlyph>(m_font, "SCORE: " + std::to_string(m_score->cScore->score), 30, sf::Color::White);
}

void Game::spawnPlayer() {
    m_player = m_entityManager.addEntity("Player");
    m_player->cTransform = std::make_shared<CTransform>((sf::Vector2f)m_window.getSize() / 2.0f, sf::Vector2f(PLAYER_SPEED, PLAYER_SPEED), 0.0f);
    m_player->cShape = std::make_shared<CShape>(PLAYER_RADIUS, PLAYER_VERTICES, PLAYER_FILL_COLOR, PLAYER_OUTLINE_COLOR, PLAYER_OUTLINE_THICKNESS);
    m_player->cInput = std::make_shared<CInput>();
    m_player->cScore = std::make_shared<CScore>();
}

void Game::spawnEnemy() {
    const float radius = m_random.randint(ENEMY_MIN_RADIUS, ENEMY_MAX_RADIUS);
    const unsigned int vertices = m_random.randint(ENEMY_MIN_VERTICES, ENEMY_MAX_VERTICES);
    const sf::Color color = { (sf::Uint8)m_random.randint(100, 250), (sf::Uint8)m_random.randint(100, 250), (sf::Uint8)m_random.randint(100, 250) };

    const sf::FloatRect bounds = { m_player->cTransform->pos.x - PLAYER_RADIUS * 6, m_player->cTransform->pos.y - PLAYER_RADIUS * 6, PLAYER_RADIUS * 12, PLAYER_RADIUS * 12 };

    sf::Vector2f pos = { m_random.randfloat(radius * 2, WINDOW_WIDTH - (radius * 2)), m_random.randfloat(radius * 2, WINDOW_WIDTH - (radius * 2)) };
    while(bounds.contains(pos)) {
        pos = { m_random.randfloat(radius * 2, WINDOW_WIDTH - (radius * 2)), m_random.randfloat(radius * 2, WINDOW_WIDTH - (radius * 2)) };
    }

    const sf::Vector2f velocity = { m_random.randfloat(ENEMY_MIN_SPEED, ENEMY_MAX_SPEED),  m_random.randfloat(ENEMY_MIN_SPEED, ENEMY_MAX_SPEED) };

    auto enemy = m_entityManager.addEntity("Enemy");
    enemy->cTransform = std::make_shared<CTransform>(pos, velocity, 0.0f);
    enemy->cShape = std::make_shared<CShape>(radius, vertices, color, ENEMY_OUTLINE_COLOR, ENEMY_OUTLINE_THICKNESS);
    enemy->cShape->circle.setScale(sf::Vector2f(1.0f, 2.0f));

    enemy->cType = std::make_shared<CType>("Tracker");
}

void Game::spawnSmallerEnemies(std::shared_ptr<Entity> entity) {
    const float angle = 360 / entity->cShape->circle.getPointCount();

    for(unsigned int i = 0; i < entity->cShape->circle.getPointCount(); i++) {
        auto smallerEnemy = m_entityManager.addEntity("Small Enemy");

        const float speed = std::sqrt(std::pow(entity->cTransform->velocity.x, 2) + std::pow(entity->cTransform->velocity.y, 2));
        const float angleRad = angle * (3.14159265 / 180.0);

        sf::Vector2f vel = {speed * std::cos(angleRad * i), speed * std::sin(angleRad * i)};

        smallerEnemy->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, vel, 0.0f);

        smallerEnemy->cShape = std::make_shared<CShape>(entity->cShape->radius / 2.0f, entity->cShape->circle.getPointCount(), entity->cShape->circle.getFillColor(), entity->cShape->circle.getOutlineColor(), entity->cShape->circle.getOutlineThickness());

        smallerEnemy->cLifeSpan = std::make_shared<CLifeSpan>(SMALLER_ENEMY_LIFESPAN);
    }
}

void Game::enemySpawner() {
    if(m_currentFrame - m_lastEnemySpawnTime == ENEMY_SPAWN_INTERVAL) {
        spawnEnemy();
        m_lastEnemySpawnTime = m_currentFrame;
    }
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const sf::Vector2f& target) {
    auto bullet = m_entityManager.addEntity("Bullet");

    sf::Vector2f velocity = target - entity->cTransform->pos;
    float mag = std::sqrt(std::pow(velocity.x, 2) + std::pow(velocity.y, 2));
    velocity = { velocity.x / mag, velocity.y / mag };
    velocity *= BULLET_SPEED;

    bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, velocity, 0.0f);
    bullet->cShape = std::make_shared<CShape>(BULLET_RADIUS, BULLET_VERTICES, BULLET_FILL_COLOR, BULLET_OUTLINE_COLOR, BULLET_OUTLINE_THICKNESS);
    bullet->cLifeSpan = std::make_shared<CLifeSpan>(BULLET_LIFESPAN);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity) {
    const int points = 15;
    const float angle = 360 / points;
    for(int i = 0; i < points; i++) {
        auto bullet = m_entityManager.addEntity("Bullet");

        const float speed = BULLET_SPEED;
        const float angleRad = angle * (3.14159265 / 180.0);

        sf::Vector2f vel = {speed * std::cos(angleRad * i), speed * std::sin(angleRad * i)};

        bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, vel, 0.0f);
        bullet->cShape = std::make_shared<CShape>(BULLET_RADIUS * 1.5f, BULLET_VERTICES, sf::Color(255, 0, 255), BULLET_OUTLINE_COLOR, BULLET_OUTLINE_THICKNESS);
        bullet->cLifeSpan = std::make_shared<CLifeSpan>(BULLET_LIFESPAN * 5);
    }
}

void Game::spawnGlyph(std::shared_ptr<Entity> entity) {
    if(entity->cTransform && entity->cShape) {
        auto glyph = m_entityManager.addEntity("Glyph"); 
        
        sf::Vector2f velocity = { entity->cTransform->velocity.x / std::abs(entity->cTransform->velocity.x), entity->cTransform->velocity.y / std::abs(entity->cTransform->velocity.y) };

        glyph->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, velocity, 0.0f);
    
        glyph->cGlyph = std::make_shared<CGlyph>(m_font, std::to_string(entity->cShape->circle.getPointCount()), entity->cShape->radius, entity->cShape->circle.getFillColor());

        glyph->cLifeSpan = std::make_shared<CLifeSpan>(SMALLER_ENEMY_LIFESPAN * 2);
    }
}

void Game::spawnParticles(std::shared_ptr<Entity> entity) {
    
    for(unsigned int i = 0; i < 10; i++) {
        auto particle = m_entityManager.addEntity("Particle");

        const sf::Vector2f velocity = { m_random.randfloat(0, 2) * 2 - 2,  m_random.randfloat(0, 2) * 2 - 2 };

        particle->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, velocity, 0.0f);
        particle->cShape = std::make_shared<CShape>(entity->cShape->radius * 0.05f, entity->cShape->circle.getPointCount(), sf::Color(200, 200, 200), sf::Color(200, 200, 200), 0.0f);
        particle->cLifeSpan = std::make_shared<CLifeSpan>(30 + m_random.randint(10, 20));
    }
}

void Game::setPaused(bool paused) {
    m_paused = !m_paused;
}

void Game::quit() {
    m_running = false;
    m_window.close();
}