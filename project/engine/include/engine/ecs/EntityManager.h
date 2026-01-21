#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include "engine/ecs/Entity.h"
#include "engine/ecs/ComponentManager.h"

namespace EisEngine{
    class Game;

    namespace ecs{
        /// \n Represents the entity management system.
        /// \n Handles the creation, referencing and deletion of entities in a game.
        class EntityManager {
        public:
            /// \n Creates an instance of the entity manager.
            /// @param componentManager - ComponentManager&: a reference to the game's component manager.
            /// @param engine - Game&: a reference to the engine instance.
            EntityManager(ComponentManager &componentManager, Game &engine);

            /// \n Creates an entity.
            /// @param name - std::string: the name of the new entity. Defaults to 'Entity'.
            /// @param tag - std::string: the tag given to the entity. Defaults to 'Untagged'.
            /// @param userData - void*: A pointer to any user-defined object. Defaults to nullptr.
            /// @return Entity& - a reference to the created entity.
            Entity &createEntity(const std::string &name = "Entity",
                                 const std::string &tag = "Untagged",
                                 void* userData = nullptr);

            /// \n Fetches an entity using its unique ID.
            /// @param guid - guid_t: the unique ID given to the wanted entity.
            /// @return Entity* - a pointer to the corresponding entity.
            [[nodiscard]] Entity *getEntity(guid_t guid);

            /// \n Fetches an entity by name.
            /// @return Entity* - a pointer to the first entity with a fitting name.
            [[nodiscard]] Entity *Find(const std::string &name);

            /// \n Fetches all entities with the given name.
            /// @return std::vector&lt;Entity*> - a vector of pointers to all corresponding entities.
            [[nodiscard]] std::vector<Entity *> FindAll(const std::string &name);

            /// \n Fetches an entity with the given tag.
            /// @return Entity* - a pointer to the first entity with a fitting tag.
            [[nodiscard]] Entity *FindWithTag(const std::string &tag);

            /// \n Fetches all entities with the given tag.
            /// @return std::vector&lt;Entity*> - a vector of pointers to all corresponding entities.
            [[nodiscard]] std::vector<Entity *> FindAllWithTag(const std::string &tag);

            /// \n Finds a component of the given type attributed to any entity.
            /// @return C*: a pointer to a component of the given type.
            /// \n Returns nullptr if none is found.
            template<typename C>
            C* FindEntityOfType(){
                std::map<guid_t, std::unique_ptr<Entity>>::iterator i;
                for (i = entities.begin(); i != entities.end(); i++) {
                    auto* component = i->second->GetComponent<C>();
                    if(component)
                        return component;
                }
                return nullptr;
            }

            /// \n Flags a specific entity for deletion.
            /// @param entity - a reference to the entity to be deleted.
            void deleteEntity(Entity &entity);
        private:
            /// \n Removes all entities from the game.
            void purgeEntities();

            /// \n A reference to the component manager.
            ComponentManager &componentManager;
            /// \n A map linking unique ID's to entities.
            std::map<guid_t, std::unique_ptr<Entity>> entities;
            /// \n A list of entities to be deleted.
            std::vector<guid_t> deleteList;
            /// \n The amount of entities created.
            int entityCounter = 0;
        };
    }
}
