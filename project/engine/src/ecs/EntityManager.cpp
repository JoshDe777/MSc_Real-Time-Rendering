#include "engine/ecs/EntityManager.h"
#include "engine/Game.h"

namespace EisEngine::ecs{
    EntityManager::EntityManager(ComponentManager &componentManager, Game &engine) : componentManager(componentManager)
    { engine.onAfterUpdate.addListener([&](Game &) { purgeEntities();});}

    Entity &EntityManager::createEntity(const std::string &name, const std::string &tag, void* userData) {
        auto guid = entityCounter++;
        auto *entity = new Entity(guid, name, componentManager, tag, userData);
        entities[guid] = std::make_unique<Entity>(*entity);
        return *getEntity(guid);
    }

    Entity *EntityManager::getEntity(const guid_t guid) {
        if(entities.find(guid) != entities.end())
            return entities.at(guid).get();
        return nullptr;
    }

    Entity *EntityManager::Find(const std::string &name) {
        std::map<guid_t, std::unique_ptr<Entity>>::iterator i;
        for (i = entities.begin(); i != entities.end(); i++)
            if (i->second->name() == name)
                return entities.at(i->first).get();
        return nullptr;
    }

    std::vector<Entity *> EntityManager::FindAll(const std::string &name){
        std::vector<Entity *> found;
        std::map<guid_t, std::unique_ptr<Entity>>::iterator i;
        for (i = entities.begin(); i != entities.end(); i++)
            if (i->second->name() == name)
                found.push_back(entities.at(i->first).get());
        return found;
    }

    Entity *EntityManager::FindWithTag(const std::string &tag) {
        std::map<guid_t, std::unique_ptr<Entity>>::iterator i;
        for (i = entities.begin(); i != entities.end(); i++)
            if (i->second->tag() == tag)
                return entities.at(i->first).get();
        return nullptr;
    }

    std::vector<Entity*> EntityManager::FindAllWithTag(const std::string &tag){
        std::vector<Entity*> found;
        std::map<guid_t, std::unique_ptr<Entity>>::iterator i;
        for (i = entities.begin(); i != entities.end(); i++)
            if (i->second->tag() == tag)
                found.push_back(entities.at(i->first).get());
        return found;
    }

    void EntityManager::deleteEntity(Entity &entity) {
        entity.deleted = true;
        entity.deleteAllComponents();
        deleteList.push_back(entity.guid());
    }

    void EntityManager::purgeEntities() {
        for(auto &guid: deleteList)
            entities.erase(guid);
        deleteList.clear();
    }
}
