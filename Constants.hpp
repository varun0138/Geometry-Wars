#pragma once

#include <SFML/Graphics/Color.hpp>

const unsigned int PLAYER_VERTICES = 8;
const float PLAYER_RADIUS = 32.0f;
const float PLAYER_SPEED = 5.0f;
const float PLAYER_OUTLINE_THICKNESS = 4.0f;
const sf::Color PLAYER_FILL_COLOR = {5, 5, 5};
const sf::Color PLAYER_OUTLINE_COLOR = {255, 0, 0};

const unsigned int ENEMY_MIN_VERTICES = 3;
const unsigned int ENEMY_MAX_VERTICES = 8;
const unsigned int ENEMY_SPAWN_INTERVAL = 90;
const float ENEMY_MIN_RADIUS = 26;
const float ENEMY_MAX_RADIUS = 40;
const float ENEMY_MIN_SPEED = 1;
const float ENEMY_MAX_SPEED = 5;
const float ENEMY_OUTLINE_THICKNESS = 2;
const float SMALLER_ENEMY_LIFESPAN = 60;

const unsigned int BULLET_VERTICES = 20;
const unsigned int BULLET_LIFEPSAN = 90;
const float BULLET_RADIUS = 7;
const float BULLET_SPEED = 20;
const float BULLET_OUTLINE_THICKNESS = 5;
const sf::Color BULLET_FILL_COLOR = {200, 200, 200};
const sf::Color BULLET_OUTLINE_COLOR = {255, 255, 255};