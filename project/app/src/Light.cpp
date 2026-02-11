#include "Light.h"

#include <random>

namespace RTR {
    inline Vector3 GetRandomOrbitAxis(){
        // get random 3D rotation vector where light will orbit around x-axis.
        // --> (1, 0, 0) = forward-backwards, up-down
        // --> (0, 1, 0) = right-left, up-down
        // --> (0, 0, 1) = right-left, forwards, backwards
        // ... and everything in-between.
        // 1 = 90° / (pi/2)

        auto x = 90.0f * (float) std::rand() / (float) RAND_MAX;
        auto y = 90.0f * (float) std::rand() / (float) RAND_MAX;
        auto z = 90.0f * (float) std::rand() / (float) RAND_MAX;
        return Vector3(x, y, z).normalized();
    }

    Light::Light(Game& game, Transform* reference, const float& orbitDist) : orbitDist(orbitDist), orbitAxis(GetRandomOrbitAxis())
    {
        // create child entity with point light features...
        auto lightEntity = game.entityManager.createEntity("light");
        lightTransform = static_cast<const shared_ptr<Transform>>(lightEntity.transform);
        mat = ResourceManager::GetMaterialInstance("default");
        mat->SetEmission(Vector3::one);
        mat->SetIntensity(intensity);
        lightEntity.AddComponent<Renderer>(nullptr, mat.get());
        component = static_cast<shared_ptr<PointLight>>(&lightEntity.AddComponent<PointLight>(mat.get()));

        // ... & offset in y direction by orbitDist.
        orbitCentre = reference->GetGlobalPosition();
        lightTransform->SetLocalPosition( orbitCentre + orbitDist * Vector3::forward);
    }

    void Light::SetColor(const Vector3 &em) {
        mat->SetEmission(em);
    }

    void Light::SetIntensity(const float &i) {
        component->SetIntensity(i);
    }

    void Light::SetOrbitVals(const Vector3& EulerXYZ){
        Quaternion orbit = Quaternion::FromEulerXYZ(EulerXYZ);
        auto dir = orbit * (-Vector3::forward);

        lightTransform->SetLocalPosition(orbitCentre + dir * orbitDist);
    }
}
