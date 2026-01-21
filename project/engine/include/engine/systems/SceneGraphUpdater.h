#pragma once

#include "engine/ecs/System.h"
#include "engine/components/Transform.h"

using EisEngine::ecs::System;
using EisEngine::components::Transform;

namespace EisEngine::systems {
    /// \n a system that updates transforms throughout the game's lifespan.
    /// updates every transform's model matrix every frame.
    class SceneGraphUpdater : public System {
    public:
        /// \n Creates an instance of the SceneGraphUpdater system.
        /// @param game - a reference to the game using the SceneGraphUpdater system.
        explicit SceneGraphUpdater(Game &game);

        /// \n Iterates through all transforms in the game and updates their model matrix.
        /// @param game - a reference to the game whose transforms are to be updated.
        static void UpdateTransforms(Game &game);
    private:
        /// \n Calculates the model matrix for a given transform.
        static glm::mat4 calculateModelMatrix(Transform &transform);
    };
}
