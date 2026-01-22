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
        shared_ptr<Transform> lightTransform;
        shared_ptr<PointLight> component;
        unique_ptr<Material> mat;
        void Orbit();
        float orbitStepPerSec = 10.0f;
        float intensity = 10.0f;
    };
}
