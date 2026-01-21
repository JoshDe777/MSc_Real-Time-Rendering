#pragma once

#include "engine/ecs/Component.h"
#include "engine/Events.h"
#include "engine/utilities/AnimState.h"

namespace EisEngine {
    using namespace ecs;
    namespace components {
        class Animator : public Component {
        public:
            explicit Animator(Game& engine, guid_t owner, std::vector<std::shared_ptr<AnimState>>& animStates);
        private:
            void Update();
            std::vector<std::shared_ptr<AnimState>> states;
            unsigned int currentState = 0;
            float elapsedTime = 0;
            bool newState = true;
        };
    }
}
