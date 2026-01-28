#pragma once

#include <EisEngine.h>
#include "engine/utilities/Quaternion.h"

namespace RTR {
    class Simulation;

    class Light {
        friend Simulation;
    public:
        explicit Light(Game& game, Transform* reference, const float& orbitDist);
        void SetColor(const Vector3& em);
        void SetIntensity(const float& i);
        void SetRotationSpeed(const float& v) {orbitStepPerSec = v;}
    private:
        shared_ptr<Transform> lightTransform;
        shared_ptr<PointLight> component;
        unique_ptr<Material> mat;
        void Orbit();
        float orbitStepPerSec = 10.0f;
        float currentOrbit = 0.0f;
        const Vector3 orbitAxis;
        Quaternion orbitFrame = Quaternion(0, 0, 0, 0);
        float orbitDist;
        float intensity = 10.0f;
        Vector3 orbitCentre = Vector3::zero;
    };
}
