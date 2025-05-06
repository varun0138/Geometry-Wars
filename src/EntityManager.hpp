#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "Entity.hpp"

using EntityVec = std::vector<std::shared_ptr<Entity>>;
using EntityMap = std::unordered_map<std::string, EntityVec>;

class EntityManager {
private:
    EntityVec m_entities;
    EntityVec m_entitiesToAdd;
    EntityMap m_entityMap;
    unsigned long long int m_totalEntities = 0;

    void removeDeadEntities(EntityVec& entityVec);

public:
    EntityManager() = default;

    void update();

    std::shared_ptr<Entity> addEntity(const std::string& tag);
    
    EntityVec& getEntities()                        { return m_entities;        }
    EntityVec& getEntities(const std::string& tag)  { return m_entityMap[tag];  }
};
