#pragma once

#include <memory>
#include <string>

#include "Components.hpp"

class Entity {
private:
    friend class EntityManager;

    unsigned int m_id = 0;
    std::string m_tag = "None";
    bool m_active = true;

    Entity(unsigned int id, const std::string& tag);

public:
    std::shared_ptr<CTransform> cTransform;
    std::shared_ptr<CShape> cShape;
    std::shared_ptr<CInput> cInput;
    std::shared_ptr<CScore> cScore;
    std::shared_ptr<CLifeSpan> cLifeSpan;
    std::shared_ptr<CGlyph> cGlyph;

    unsigned int id() const;
    std::string tag() const;
    bool isActive() const;
    void destroy();
};