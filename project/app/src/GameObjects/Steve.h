#pragma once

#include <EisEngine.h>

namespace Maze {
    class Steve {
    public:
        explicit Steve(Game& game);
        void Animate();
        Entity* entity = nullptr;
    private:
        Entity* torso = nullptr;
        Entity* neck = nullptr;
        Entity* shoulderL = nullptr;
        Entity* shoulderR = nullptr;
        Entity* hipL = nullptr;
        Entity* hipR = nullptr;
        Entity* dynamic_light = nullptr;

        float animSpeed = 5;
        float stride = 25.0f;
        float moveSpeed = 0.25f;

        float animTime = 0;
    };
}
