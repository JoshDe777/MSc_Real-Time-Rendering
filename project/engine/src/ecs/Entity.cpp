#include <utility>

#include "engine/ecs/Entity.h"

namespace EisEngine::ecs{
    Entity::Entity(EisEngine::ecs::guid_t id, std::string name, EisEngine::ecs::ComponentManager &componentManager,
                   std::string tag, void* userData)  : m_id(id), m_name(std::move(name)), m_tag(std::move(tag)),
                   componentManager(componentManager), user_data(userData) {
        if(m_id < 0)
            DEBUG_WARN("<Entity::Entity> Invalid Entity ID")
        transform = &AddComponent<Transform>();
    }

    template<typename C>
    void Entity::RemoveComponent(Component &component) {
        component.Invalidate();
        componentManager.removeComponent<C>(m_id);
    }

    template<typename C>
    void Entity::RemoveComponent() {
        auto &component = GetComponent<C>();
        RemoveComponent<C>(component);
    }
}
