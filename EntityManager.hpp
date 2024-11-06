#pragma once

#include <vector>
#include <map>
#include <memory>

#include "Entity.hpp"

using EntityVec = std::vector<std::shared_ptr<Entity>>;
using EntityMap = std::map<std::string, EntityVec>;

class EntityManager {
private:
    EntityVec m_entityVec;
    EntityVec m_entitiesToAdd;
    EntityMap m_entityMap;
    unsigned long long int m_totalEntities = 0;

    void removeDeadEntities(EntityVec& entityVec);

public:
    EntityManager();

    void update();

    std::shared_ptr<Entity> addEntity(const std::string& tag);
    const EntityVec& getEntities();
    const EntityVec& getEntities(const std::string& tag);
};
