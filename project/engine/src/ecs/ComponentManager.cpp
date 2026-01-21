#pragma once

#include "engine/ecs/ComponentManager.h"
#include "engine/Game.h"

namespace EisEngine::ecs{
    ComponentManager::ComponentManager(Game &engine) : engine(engine)
    { engine.onAfterUpdate.addListener([&](Game &) { purgeComponents();});}
}
