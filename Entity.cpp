#include "Entity.hpp"

Entity::Entity(unsigned int id, const std::string& tag) 
    : m_id(id), m_tag(tag) {}

unsigned int Entity::id() const {
    return m_id;
}

std::string Entity::tag() const {
    return m_tag;
}

bool Entity::isActive() const {
    return m_active;
}

void Entity::destroy() {
    m_active = false;
}