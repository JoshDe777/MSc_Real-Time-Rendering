#pragma once

#include <EisEngine.h>
#include <FastNoiseLite.h>

namespace Maze {
    class Torch {
    public:
        explicit Torch(Game& game);
        Entity* entity = nullptr;
    private:
        unique_ptr<FastNoiseLite> noise;
        Renderer* flame = nullptr;
        float flickerSpeed = 25;
        float animTime = 0.0f;
        float maxIntensity = 0.25f;
        void Flicker();
    };
}
