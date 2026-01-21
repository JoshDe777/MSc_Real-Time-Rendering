#pragma once

#include "engine/ecs/System.h"
#include "engine/components/Transform.h"

namespace EisEngine {
    using namespace EisEngine::ecs;
    namespace systems{

        /// \n A system taking for deleting unwanted Transform components.
        /// Searches for Transform instances flagged for deletion and deletes the transform and its associated entity.
        class SceneGraphPruner : public System {
        public:
            /// \n Creates an instance of the SceneGraphPruner system.
            /// @param engine - a reference to the game using the SceneGraphPruner system.
            explicit SceneGraphPruner(Game &engine);

            /// \n Iterates through all transforms and deletes the entities of transforms flagged for deletion.
            /// @param engine - a reference to the game using the system.
            void PruneTransforms(Game &engine);
        };
    }
}
