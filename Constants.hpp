#pragma once

#include <SFML/Graphics/Color.hpp>

const unsigned int WINDOW_WIDTH = 1680;
const unsigned int WINDOW_HEIGHT = 945;

const unsigned int PLAYER_VERTICES = 8;
const float PLAYER_RADIUS = 32.0f;
const float PLAYER_SPEED = 8.0f;
const float PLAYER_OUTLINE_THICKNESS = 4.0f;
const sf::Color PLAYER_FILL_COLOR = {5, 5, 5};
const sf::Color PLAYER_OUTLINE_COLOR = {255, 0, 0};

const unsigned int ENEMY_MIN_VERTICES = 3;
const unsigned int ENEMY_MAX_VERTICES = 8;
const unsigned int ENEMY_SPAWN_INTERVAL = 60;
const float ENEMY_MIN_RADIUS = 26;
const float ENEMY_MAX_RADIUS = 40;
const float ENEMY_MIN_SPEED = 1;
const float ENEMY_MAX_SPEED = 5;
const float ENEMY_OUTLINE_THICKNESS = 2;
const float SMALLER_ENEMY_LIFESPAN = 60;
const sf::Color ENEMY_OUTLINE_COLOR = {255, 255, 255};

const unsigned int BULLET_VERTICES = 20;
const unsigned int BULLET_LIFESPAN = 30;
const float BULLET_RADIUS = 6;
const float BULLET_SPEED = 20;
const float BULLET_OUTLINE_THICKNESS = 3;
const sf::Color BULLET_FILL_COLOR = {225, 225, 225};
const sf::Color BULLET_OUTLINE_COLOR = {255, 255, 255};

const unsigned int STAR_MIN_VERTICES = 14;
const unsigned int STAR_MAX_VERTICES = 30;
const sf::Color STAR_COLOR = {213, 224, 216};

const unsigned int HUD_FONT_SIZE = 40;
const sf::Color HUD_FONT_COLOR = {190, 212, 85};