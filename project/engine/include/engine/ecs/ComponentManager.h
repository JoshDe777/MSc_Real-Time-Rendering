#pragma once

#include <map>
#include <vector>
#include <memory>
#include <typeinfo>
#include <utility>
#include <functional>
#include <string>
#include "engine/ecs/ecs.h"
#include "engine/ecs/Component.h"

namespace EisEngine {
    class Game;

    namespace ecs {
        /// \n Represents the Component management system.
        /// Handles allocating and deallocating components to entities as well as the deletion process for components of all kind.
        class ComponentManager {
        public:
            /// \n Creates a @a ComponentManager instance
            /// @param engine a reference to the game's instance.
            explicit ComponentManager(Game &engine);

            /// \n Creates a component of the given type and assigns it to an entity.
            /// @return Component& - a reference to the newly created Component.
            template<typename C, typename ...Args>
            [[nodiscard]] C &addComponent(guid_t owner, Args ...args){
                auto& container = containers[typeid(C).hash_code()];
                auto component = std::make_unique<C>(engine, owner, args...);
                container[owner] = std::move(component);

                return *getComponent<C>(owner);
            }

            /// \n Gets a Component of the given type from the specified entity.
            /// @return Component* - a pointer to the component if found on the owner entity.
            /// \n Returns a nullptr if no component was found.
            template<typename C>
            C *getComponent(guid_t owner){
                // return nullptr if no component of given type or if none assigned to the owner id
                if (!hasComponentOfType<C>() ||
                        containers.at(typeid(C).hash_code()).find(owner) ==
                        containers.at(typeid(C).hash_code()).end()
                        )
                    return nullptr;
                try {
                    return reinterpret_cast<C*>(containers.at(typeid(C).hash_code()).at(owner).get());
                }
                catch (std::exception &e){
                    DEBUG_ERROR("ComponentManager::getComponent<" + (std::string) typeid(C).name() + ">" + e.what())
                    return nullptr;
                }
            }

            /// \n Iterates through all components of a given type and executes a function on all of them.
            /// @param C - the (sub-)type of Component to be iterated through.
            /// @param f - a function with return type @a null that will be executed on all components.
            template<typename C>
            void forEachComponent(std::function<void(C&)> f){
                if (!hasComponentOfType<C>())
                    return;
                for(const auto &[_, component]: containers.at(typeid(C).hash_code())) {
                    f(*reinterpret_cast<C*>(component.get()));
                }
            }


            /// \n Removes a Component from the given entity.
            /// @param entityID - the unique ID of the entity whose component is to be removed.
            template<typename C>
            void removeComponent(guid_t entityID){
                auto& componentContainer = containers[typeid(C).hash_code()];
                const auto &component = componentContainer.find(entityID);
                const auto componentExistsInContainer = component != componentContainer.end();

                if(componentExistsInContainer) {
                    component->second->deleted = true;
                    component->second->Invalidate();
                    componentContainer.erase(entityID);
                }
            }

            /// \n Returns each component assigned to the given entity.
            /// ASSUMPTION ONLY ONE COMPONENT OF ANY TYPE PER ENTITY!
            std::vector<Component*> getEachComponentOfEntity(guid_t entityID){
                std::vector<Component*> components = {};
                for(auto &[componentTypeID, componentContainer] : containers){
                    const auto& component = componentContainer.find(entityID);
                    if(component != componentContainer.end())
                        components.emplace_back(&*component->second);
                }
                return components;
            }

            /// \n Removes all components from the given entity.
            /// @param entityID - the unique ID of the entity whose components are to be deleted.
            void removeComponents(guid_t entityID){
                for(auto &[componentTypeID, componentContainer]: containers) {
                    const auto &component = componentContainer.find(entityID);
                    const auto componentExistsInContainer = component != componentContainer.end();
                    if(componentExistsInContainer) {
                        component->second->deleted = true;
                        componentContainer.erase(entityID);
                    }
                }
            }

            /// \n determines whether there is an existing component of the given type.
            template<typename C>
            bool hasComponentOfType() { return containers.count(typeid(C).hash_code()) != 0;}
        private:
            using ComponentContainer = std::map<guid_t, std::unique_ptr<Component>>;

            /// \n A dictionary of component containers.
            /// \n Maps a container for a type X of component to its hashed typeid
            std::map<size_t, ComponentContainer> containers;
            /// \n A reference to the engine instance to pass on to components.
            Game &engine;
        };
    }
}
