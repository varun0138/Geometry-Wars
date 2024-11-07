#pragma once

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>

class CTransform {
public:
    sf::Vector2f pos = { 0.0f, 0.0f };
    sf::Vector2f velocity = { 0.0f, 0.0f };
    float angle = 0.0f;

    CTransform(const sf::Vector2f& p, const sf::Vector2f& v, float a) 
        : pos(p), velocity(v), angle(a) {}
};

class CShape {
public:
    sf::CircleShape circle;

    CShape(float radius, unsigned int points, const sf::Color& fill, const sf::Color& outline, float thickness) 
        : circle(radius, points) {

        circle.setFillColor(fill);
        circle.setOutlineColor(outline);
        circle.setOutlineThickness(thickness);
        circle.setOrigin(sf::Vector2f(radius, radius));
    }
};

class CCollision {
public:
    float radius = 0.0f;

    CCollision(float r)
        : radius(r) {} 
};

class CInput {
public:
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool shoot = false;

    CInput() {}
};

class CScore {
public:
    unsigned int score = 0;

    CScore(unsigned int s = 0) 
        : score(s) {}
};

class CLifeSpan {
public:
    unsigned int total = 0;
    unsigned int remaining = 0;

    CLifeSpan(unsigned int t)
        : total(t), remaining(t) {}
};

class CType {
public:
    std::string type = "None";

    CType(const std::string& t)
        : type(t) {}
};