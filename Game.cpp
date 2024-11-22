#include "Game.hpp"
#include "Constants.hpp"

#include <SFML/Window/Event.hpp>
#include <cmath>
#include <iostream>
#include <fstream>

Game::Game() {
    m_window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "", sf::Style::Close | sf::Style::Titlebar);
    m_window.setPosition(sf::Vector2i(100, 5));
    m_window.setFramerateLimit(60);

    m_assets.loadFromFile("./assets.txt");

    setUp();
}

void Game::setUp() {
    for(unsigned int i = 0; i < 200; i++) {
        spawnStar();
    }

    spawnPlayer();
    spawnScore();
    spawnHighScore();
    spawnLives();
    spawnStartTimer();
    m_assets.getSound("HighLife").play();
    m_assets.getSound("HighLife").setLoop(true);
}

void Game::run() {
    while(m_running) {
        
        m_entityManager.update();
        sUserInput();

        if(!m_paused && m_startTimerRemaining <= 0) {
            enemySpawner();
            sMovement();
            sTrail();
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
                    m_assets.getSound("ShotgunShoot").play();
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
        m_assets.getSound("Shoot").play();
        m_player->cInput->shoot = false;
    }

    for(auto& enemy: m_entityManager.getEntities("Enemy")) {
        sf::Vector2f velocity = m_player->cTransform->pos - enemy->cTransform->pos;
        float mag = std::sqrt(std::pow(velocity.x, 2) + std::pow(velocity.y, 2));
        velocity = { velocity.x / mag, velocity.y / mag };
        velocity *= ENEMY_MAX_SPEED;
        enemy->cTransform->velocity = velocity;
    }

    for(auto& entity: m_entityManager.getEntities()) {
        if(entity->cTransform) {
            entity->cTransform->prevPos = entity->cTransform->pos;
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
        if(m_player->cInvincibility->iFrames == 0 && distance <= radii) {
            m_player->cInvincibility->iFrames = PLAYER_INVINCIBILITY;
            m_lives->cScore->score--;
            m_lives->cGlyph->text.setString("LIVES: " + std::to_string(m_lives->cScore->score));
            if(m_lives->cScore->score <= 0) {
                setHighScore();
                quit();
            }
        }
        else if(distance <= radii) {
            enemy->destroy();
            m_assets.getSound("MachineHit").play();
            spawnSmallerEnemies(enemy);
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
                if(m_score->cScore->score > m_highScore->cScore->score) {
                    if(m_brokeHighScore) {
                        spawnNewHighScore();
                        m_brokeHighScore = false;
                    }
                    m_highScore->cScore->score = m_score->cScore->score;
                    m_highScore->cGlyph->text.setString("HIGH SCORE: " + std::to_string(m_highScore->cScore->score));
                }
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

                if(entity->tag() == "Star") {
                    spawnStar();
                }

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

    // PLAYER HIT EFFECT
    if(m_player->cInvincibility && m_player->cInvincibility->iFrames > 0) {
        m_player->cInvincibility->iFrames--;
        m_assets.getSound("PlayerHit").play();

        // Flash effect: toggle between red and original color
        if (m_player->cInvincibility->iFrames % 10 < 5) {
            m_player->cShape->circle.setFillColor(sf::Color(255, 255, 255, 127)); // Flash color
            m_player->cShape->radius -= 1;
        } else {
            m_player->cShape->circle.setFillColor(sf::Color(PLAYER_OUTLINE_COLOR.r, PLAYER_OUTLINE_COLOR.g, PLAYER_OUTLINE_COLOR.b, 127)); // Original color
            m_player->cShape->radius += 1;
        }

        if(m_player->cInvincibility->iFrames == 0) {
            m_player->cShape->circle.setFillColor(PLAYER_FILL_COLOR);
            m_player->cShape->radius = PLAYER_RADIUS;
        }
    }
}

void Game::sRender() {
    m_window.clear();

    for(auto& entity: m_entityManager.getEntities("Star")) {
        if(entity->cTransform && entity->cShape) {
            entity->cShape->circle.setPosition(entity->cTransform->pos);
            entity->cShape->circle.setRadius(entity->cShape->radius);

            entity->cTransform->angle += 1.0f;
            entity->cShape->circle.setRotation(entity->cTransform->angle);

            m_window.draw(entity->cShape->circle);
        }
    }

    for(auto& entity: m_entityManager.getEntities("Particle")) {
        if(entity->cTransform && entity->cShape) {
            entity->cShape->circle.setPosition(entity->cTransform->pos);
            entity->cShape->circle.setRadius(entity->cShape->radius);

            entity->cTransform->angle += 1.0f;
            entity->cShape->circle.setRotation(entity->cTransform->angle);

            m_window.draw(entity->cShape->circle);
        }
    }

    for(auto& entity: m_entityManager.getEntities("Enemy")) {
        if(entity->cTransform && entity->cShape) {
            entity->cShape->circle.setPosition(entity->cTransform->pos);
            entity->cShape->circle.setRadius(entity->cShape->radius);

            entity->cTransform->angle += 1.0f;
            entity->cShape->circle.setRotation(entity->cTransform->angle);

            m_window.draw(entity->cShape->circle);
        }
    }

    for(auto& entity: m_entityManager.getEntities("Small Enemy")) {
        if(entity->cTransform && entity->cShape) {
            entity->cShape->circle.setPosition(entity->cTransform->pos);
            entity->cShape->circle.setRadius(entity->cShape->radius);

            entity->cTransform->angle += 1.0f;
            entity->cShape->circle.setRotation(entity->cTransform->angle);

            m_window.draw(entity->cShape->circle);
        }
    }

    for(auto& entity: m_entityManager.getEntities("Bullet")) {
        if(entity->cTransform && entity->cShape) {
            entity->cShape->circle.setPosition(entity->cTransform->pos);
            entity->cShape->circle.setRadius(entity->cShape->radius);

            entity->cTransform->angle += 1.0f;
            entity->cShape->circle.setRotation(entity->cTransform->angle);

            m_window.draw(entity->cShape->circle);
        }
    }

    for(auto& entity: m_entityManager.getEntities("Trail")) {
        if(entity->cTransform && entity->cShape) {
            entity->cShape->circle.setPosition(entity->cTransform->pos);
            entity->cShape->circle.setRadius(entity->cShape->radius);

            entity->cTransform->angle += 1.0f;
            entity->cShape->circle.setRotation(entity->cTransform->angle);

            m_window.draw(entity->cShape->circle);
        }
    }

    for(auto& entity: m_entityManager.getEntities("Player")) {
        if(entity->cTransform && entity->cShape) {
            entity->cShape->circle.setPosition(entity->cTransform->pos);
            entity->cShape->circle.setRadius(entity->cShape->radius);

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

    if(m_startTimer) {
        m_startTimerRemaining = m_startTimerTotal - (unsigned int)m_clock.getElapsedTime().asSeconds();

        if(m_startTimerRemaining > 0) {
            m_timer->cGlyph->text.setString(std::to_string(m_startTimerRemaining));
            for(auto& entity: m_entityManager.getEntities("Start Timer")) {
                if(entity->cGlyph) {
                    entity->cGlyph->text.setPosition(entity->cTransform->pos);

                    m_window.draw(entity->cGlyph->text);
                }
            }
        }
        else {
            m_startTimer = false;
        }
    }

    m_window.display();
}

void Game::spawnNewHighScore() {
    auto score = m_entityManager.addEntity("Glyph"); 

    score->cGlyph = std::make_shared<CGlyph>(m_assets.getFont("GoldenAgeShad"), "NEW HIGH SCORE!!", NEW_HIGHSCORE_FONT_SIZE, NEW_HIGHSCORE_FONT_COLOR);

    sf::FloatRect bounds = score->cGlyph->text.getLocalBounds();
    score->cGlyph->text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);

    score->cTransform = std::make_shared<CTransform>(sf::Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f), sf::Vector2f(0.0f, 0.0f), 0.0f);

    score->cLifeSpan = std::make_shared<CLifeSpan>(NEW_HIGHSCORE_LIFESPAN);
}

void Game::spawnStartTimer() {
    m_timer = m_entityManager.addEntity("Start Timer"); 

    m_timer->cGlyph = std::make_shared<CGlyph>(m_assets.getFont("GoldenAgeShad"), std::to_string(m_startTimerRemaining), START_TIMER_FONT_SIZE, START_TIMER_FONT_COLOR);

    sf::FloatRect bounds = m_timer->cGlyph->text.getLocalBounds();
    m_timer->cGlyph->text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);

    m_timer->cTransform = std::make_shared<CTransform>(sf::Vector2f(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f), sf::Vector2f(0.0f, 0.0f), 0.0f);
}

void Game::sTrail() {
    for(auto& bullet: m_entityManager.getEntities("Bullet")) {
        auto trail = m_entityManager.addEntity("Trail");

        sf::Color fillColor = bullet->cShape->circle.getFillColor();
        sf::Color outLine = bullet->cShape->circle.getOutlineColor();

        trail->cLifeSpan = std::make_shared<CLifeSpan>(bullet->cLifeSpan->remaining * 0.1f);
        trail->cTransform = std::make_shared<CTransform>(bullet->cTransform->prevPos, sf::Vector2f(0.0f, 0.0f), 0.0f);
        trail->cShape = std::make_shared<CShape>(bullet->cShape->radius, bullet->cShape->circle.getPointCount(), sf::Color(fillColor.r, fillColor.g, fillColor.b, 50), sf::Color(outLine.r, outLine.g, outLine.b, 50), bullet->cShape->circle.getOutlineThickness());
    }
}

void Game::spawnStar() {
    auto star = m_entityManager.addEntity("Star");

    star->cTransform = std::make_shared<CTransform>(sf::Vector2f(m_random.randint(0, WINDOW_WIDTH), m_random.randint(0, WINDOW_HEIGHT)), sf::Vector2f(m_random.randfloat(0, 2) * 4 - 2, m_random.randfloat(0, 2) * 3 - 1), 0.0f);
    star->cShape = std::make_shared<CShape>(m_random.randfloat(1, 2), m_random.randint(STAR_MIN_VERTICES, STAR_MAX_VERTICES), STAR_COLOR, STAR_COLOR, 0.0f);
    star->cLifeSpan = std::make_shared<CLifeSpan>(m_random.randint(100, 200));
}

void Game::spawnScore() {
    m_score = m_entityManager.addEntity("Glyph"); 

    m_score->cTransform = std::make_shared<CTransform>(sf::Vector2f(5.0f, 5.0f), sf::Vector2f(0.0f, 0.0f), 0.0f);
    m_score->cScore = std::make_shared<CScore>(0);
    m_score->cGlyph = std::make_shared<CGlyph>(m_assets.getFont("GoldenAgeShad"), "SCORE: " + std::to_string(m_score->cScore->score), HUD_FONT_SIZE, HUD_FONT_COLOR);
}

void Game::spawnHighScore() {
    std::ifstream file("./Stats.ini");
    if(!file) {
        std::cerr << "Error loading Stats file!!" << std::endl;
    }
    unsigned int value;
    file >> value;
    
    file.close();

    m_highScore = m_entityManager.addEntity("Glyph"); 

    m_highScore->cTransform = std::make_shared<CTransform>(sf::Vector2f(1290.0f, 5.0f), sf::Vector2f(0.0f, 0.0f), 0.0f);
    m_highScore->cScore = std::make_shared<CScore>(value);
    m_highScore->cGlyph = std::make_shared<CGlyph>(m_assets.getFont("GoldenAgeShad"), "HIGH SCORE: " + std::to_string(m_highScore->cScore->score), HUD_FONT_SIZE, HUD_FONT_COLOR);
}

void Game::setHighScore() {
    std::ofstream file("./Stats.ini");
    if(!file) {
        std::cerr << "Error loading Stats file!!" << std::endl;
    }
    file << m_highScore->cScore->score;
    
    file.close();
}

void Game::spawnLives() {
    m_lives = m_entityManager.addEntity("Glyph"); 

    m_lives->cTransform = std::make_shared<CTransform>(sf::Vector2f(700.0f, 5.0f), sf::Vector2f(0.0f, 0.0f), 0.0f);
    m_lives->cScore = std::make_shared<CScore>(5);
    m_lives->cGlyph = std::make_shared<CGlyph>(m_assets.getFont("GoldenAgeShad"), "LIVES: " + std::to_string(m_lives->cScore->score), HUD_FONT_SIZE, HUD_FONT_COLOR);
}   

void Game::spawnPlayer() {
    m_player = m_entityManager.addEntity("Player");
    m_player->cTransform = std::make_shared<CTransform>((sf::Vector2f)m_window.getSize() / 2.0f, sf::Vector2f(PLAYER_SPEED, PLAYER_SPEED), 0.0f);
    m_player->cShape = std::make_shared<CShape>(PLAYER_RADIUS, PLAYER_VERTICES, PLAYER_FILL_COLOR, PLAYER_OUTLINE_COLOR, PLAYER_OUTLINE_THICKNESS);
    m_player->cInput = std::make_shared<CInput>();
    m_player->cScore = std::make_shared<CScore>();
    m_player->cInvincibility = std::make_shared<CInvincibility>();
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
        bullet->cShape = std::make_shared<CShape>(BULLET_RADIUS * m_random.randfloat(1.0f, 1.5f), PLAYER_VERTICES, sf::Color(212, 131, 212), sf::Color(212, 131, 212), BULLET_OUTLINE_THICKNESS);
        bullet->cLifeSpan = std::make_shared<CLifeSpan>(BULLET_LIFESPAN * 5);
    }
}

void Game::spawnGlyph(std::shared_ptr<Entity> entity) {
    if(entity->cTransform && entity->cShape) {
        auto glyph = m_entityManager.addEntity("Glyph"); 
        
        sf::Vector2f velocity = { entity->cTransform->velocity.x / std::abs(entity->cTransform->velocity.x), entity->cTransform->velocity.y / std::abs(entity->cTransform->velocity.y) };

        glyph->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, velocity, 0.0f);
    
        glyph->cGlyph = std::make_shared<CGlyph>(m_assets.getFont("GoldenAgeShad"), std::to_string(entity->cShape->circle.getPointCount()), entity->cShape->radius, entity->cShape->circle.getFillColor());

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