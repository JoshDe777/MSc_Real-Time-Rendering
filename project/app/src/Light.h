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
        void SetOrbitVals(const Vector3& EulerXYZ);
        void SetOrbitDist(const float& dist) { orbitDist = dist;}
    private:
        shared_ptr<Transform> lightTransform;
        shared_ptr<PointLight> component;
        unique_ptr<Material> mat;
        const Vector3 orbitAxis;
        float orbitDist;
        float intensity = 10.0f;
        Vector3 orbitCentre = Vector3::zero;
    };
}
