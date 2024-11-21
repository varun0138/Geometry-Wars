#pragma once

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

class CTransform {
public:
    sf::Vector2f pos = { 0.0f, 0.0f };
    sf::Vector2f prevPos = { 0.0f, 0.0f };
    sf::Vector2f velocity = { 0.0f, 0.0f };
    float angle = 0.0f;

    CTransform(const sf::Vector2f& p, const sf::Vector2f& v, float a) 
        : pos(p), prevPos(p), velocity(v), angle(a) {}
};

class CShape {
public:
    sf::CircleShape circle;
    float radius;

    CShape(float rad, unsigned int points, const sf::Color& fill, const sf::Color& outline, float thickness) 
        : circle(rad, points), radius(rad) {

        circle.setFillColor(fill);
        circle.setOutlineColor(outline);
        circle.setOutlineThickness(thickness);
        circle.setOrigin(sf::Vector2f(radius, radius));
    }
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


class CGlyph {
public:
    sf::Text text;

    CGlyph(const sf::Font& font, const std::string& caption, float size, const sf::Color& color) {
        text.setFont(font);
        text.setString(caption);
        text.setCharacterSize(size);
        text.setFillColor(color);
    }
};

class CInvincibility {
public:
    unsigned int iFrames = 0;

    CInvincibility(unsigned int f=0)
        : iFrames(f) {}
};