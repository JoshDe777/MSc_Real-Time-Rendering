#pragma once

#include <string>
#include <utility>
#include <vector>
#include "ecs.h"
#include "engine/ecs/ComponentManager.h"
#include "engine/components/Transform.h"

namespace EisEngine::ecs {
    /// \n Represents an object of a game with data and/or logic to it.
    class Entity final {
        friend class EntityManager;
        using Transform = EisEngine::components::Transform;
    public:
        /// \n Gets the entity's unique ID.
        [[nodiscard]] guid_t guid() const { return m_id; }

        /// \n Determines whether an entity is marked for deletion.
        [[nodiscard]] bool isDeleted() const { return deleted; }
        /// \n Checks if the given string matches the entity's tag.
        [[nodiscard]] bool CompareTag(const std::string& tag) { return tag == m_tag;}

        /// \n Gets the entity's name.
        [[nodiscard]] std::string name() const {return m_name;}
        /// \n Gets the entity's tag.
        [[nodiscard]] std::string tag() const {return m_tag;}

        /// \n Changes the entity's tag.
        void setTag(const std::string &tag) { m_tag = tag;}
        /// \n Changes the entity's name.
        void rename(const std::string &name){ m_name = name;}

        /// \n Adds a component of the given type to an entity.
        /// @return @a Component& - a reference to the newly instantiated Component.
        template<typename C, typename ...Args>
        C &AddComponent(Args ...args) { return componentManager.addComponent<C>(m_id, args...);}

        /// \n Gets a component from this entity.
        /// @return Component* - a pointer to the retrieved Component.
        /// \n returns nullptr if none is found.
        template<typename C>
        C *GetComponent() { return componentManager.getComponent<C>(m_id);}

        /// \n Removes a specific component from this entity.
        /// @param C - the type of component to be removed.
        /// @param component - a reference to the exact component to be removed.
        template<typename C>
        void RemoveComponent(Component &component);

        /// \n Removes any component from this entity.
        /// @param C - the type of component to be removed.
        template<typename C>
        void RemoveComponent();

        /// \n a pointer to the transform assigned to an entity.
        Transform* transform = nullptr;
        /// \n A void pointer allowing for users to bridge between their systems and an EisEngine entity.
        void* user_data = nullptr;

        bool operator==(Entity& other) const {return other.m_id == m_id;}
    private:
        /// \n Creates a new entity.
        /// @param name - std::string: the name given to the entity, not necessarily unique.
        /// \n defaults to "Entity".
        /// @param tag - std::string: the tag given to the entity.
        /// \n defaults to "Untagged".
        explicit Entity(guid_t id,
                        std::string name,
                        ComponentManager &componentManager,
                        std::string  tag = "Untagged",
                        void* userData = nullptr);

        /// \n Cleanses all components assigned to this entity.
        void deleteAllComponents() {
            for(auto component : componentManager.getEachComponentOfEntity(m_id))
                component->Invalidate();
            componentManager.removeComponents(m_id);}

        /// \n the unique ID of this entity.
        guid_t m_id = invalidID;
        /// \n the name of this entity.
        std::string m_name;
        /// \n the tag of this entity.
        std::string m_tag;
        /// \n Determines whether the entity is marked for deletion.
        bool deleted = false;
        /// \n The component manager, used to create, get and remove components.
        ComponentManager &componentManager;
    };
}
