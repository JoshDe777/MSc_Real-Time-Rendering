#pragma once

#include "engine/systems/SceneGraphPruner.h"
#include "engine/Game.h"

namespace EisEngine::systems{
    SceneGraphPruner::SceneGraphPruner(EisEngine::Game &game)  : System(game)
    { game.onBeforeUpdate.addListener([&] (Game &engine) { PruneTransforms(engine);});}

    void SceneGraphPruner::PruneTransforms(EisEngine::Game &engine) {
        if(!engine.componentManager.hasComponentOfType<Transform>())
            return;
        engine.componentManager.forEachComponent<Transform>([&] (Transform &transform){
            if (transform.isDeleted() && !transform.entity()->isDeleted()){
                auto entity = transform.entity();
                engine.entityManager.deleteEntity(*entity);
            }
        });
    }
}
