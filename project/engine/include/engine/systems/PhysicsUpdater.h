#pragma once

#include "engine/ecs/System.h"

namespace EisEngine::systems {
    using ecs::System;

    /// \n This system is designed to synchronize physics bodies to their transforms.
    class PhysicsUpdater : public System {
    public:
        /// \n Creates a Physics Updater instance
        /// @param engine - a reference to the engine instance to access components and entities.
        explicit PhysicsUpdater(Game &engine);

        /// \n Is called every frame to synchronize every Body component with its corresponding transform.
        static void SyncBodiesToTransforms(Game &engine);
    };
}
