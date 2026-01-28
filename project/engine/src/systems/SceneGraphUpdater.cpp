#pragma once

#include "engine/systems/SceneGraphUpdater.h"
#include "engine/Game.h"

#include <glm/gtx/euler_angles.hpp>

namespace EisEngine::systems{

    SceneGraphUpdater::SceneGraphUpdater(Game &game) : System(game)
    {
        game.onBeforeUpdate.addListener([&] (Game &game){ UpdateTransforms(game);});
    }

    void SceneGraphUpdater::UpdateTransforms(EisEngine::Game &game) {
        if(!game.componentManager.hasComponentOfType<Transform>())
            return;

        game.componentManager.forEachComponent<Transform>([&] (Transform &transform){
            if(!transform.IsDirty())
                return;

            glm::mat4 oldMatrix = transform.modelMatrix;
            transform.modelMatrix = calculateModelMatrix(transform);
        });
    }

    glm::mat4 SceneGraphUpdater::calculateModelMatrix(EisEngine::components::Transform &transform) {
        if(transform.deleted)
            return glm::mat4(1.0f);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, (glm::vec3) transform.GetLocalPosition());
        auto rotationVec3 = transform.GetLocalRotation();
        model *= glm::eulerAngleYXZ(
                glm::radians(rotationVec3.y),
                glm::radians(rotationVec3.x),
                glm::radians(rotationVec3.z)
            );
        model = glm::scale(model, (glm::vec3) transform.GetLocalScale());
        transform.dirty = false;
        return model;
    }
}