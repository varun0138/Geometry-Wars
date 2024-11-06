#include "EntityManager.hpp"


EntityManager::EntityManager() {}

void EntityManager::removeDeadEntities(EntityVec& entityVec) {
    auto isDead = [](std::shared_ptr<Entity> entity) { return !entity->isActive();};
    entityVec.erase(std::remove_if(entityVec.begin(), entityVec.end(), isDead), entityVec.end());
}

void EntityManager::update() {
    for(unsigned int i = 0; i < m_entitiesToAdd.size(); i++) {
        m_entityVec.push_back(m_entitiesToAdd[i]);
        m_entityMap[m_entitiesToAdd[i]->tag()].push_back(m_entitiesToAdd[i]);
    }
    m_entitiesToAdd.clear();
    
    removeDeadEntities(m_entityVec);

    for(auto& value: m_entityMap) {
        removeDeadEntities(value.second);
    }
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag) {
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
    m_entitiesToAdd.push_back(entity);
    return entity;
}

const EntityVec& EntityManager::getEntities() {
    return m_entityVec;
}

const EntityVec& EntityManager::getEntities(const std::string& tag) {
    return m_entityMap[tag];
}