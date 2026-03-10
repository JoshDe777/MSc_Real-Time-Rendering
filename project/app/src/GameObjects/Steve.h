#pragma once

#include <EisEngine.h>

namespace Maze {
    class Steve {
    public:
        explicit Steve(Game& game);
        void Animate();
        shared_ptr<Entity> entity = nullptr;
    private:
        shared_ptr<Entity> torso = nullptr;
        shared_ptr<Entity> neck = nullptr;
        shared_ptr<Entity> shoulderL = nullptr;
        shared_ptr<Entity> shoulderR = nullptr;
        shared_ptr<Entity> hipL = nullptr;
        shared_ptr<Entity> hipR = nullptr;

        float animSpeed = 5;
        float stride = 25.0f;
        float moveSpeed = 0.025f;

        float animTime = 0;
    };
}
