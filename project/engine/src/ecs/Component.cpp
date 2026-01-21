#pragma once

#include "engine/ecs/Component.h"
#include "engine/Game.h"

namespace EisEngine::ecs{
    Component::Component(EisEngine::Game &engine, EisEngine::ecs::guid_t owner) : owner(owner), engine(engine) { }

    Entity *Component::entity() const { return engine.entityManager.getEntity(owner);}
}
