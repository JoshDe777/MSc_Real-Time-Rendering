#pragma once

#include <EisEngine.h>

namespace RTR {
    class Simulation;

    class Light {
        friend Simulation;
    public:
        explicit Light(Game& game, Transform* reference, const float& orbitDist);
    private:
        shared_ptr<Entity> entity;
        void Orbit();
        float orbitStepPerSec = 5.0f;
        float intensity = 1.0f;
    };
}
