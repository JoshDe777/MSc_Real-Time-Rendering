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
        return Vector3(x, y, z);
    }

    Light::Light(Game& game, Transform* reference, const float& orbitDist)
    {
        // set empty entity at reference position
        entity = std::make_shared<Entity>(game.entityManager.createEntity("lightRef"));

        // set entity rotation to axis
        entity->transform->SetGlobalPosition(reference->GetGlobalPosition());
        entity->transform->SetLocalRotation(GetRandomOrbitAxis());

        // create child entity with point light features...
        auto lightEntity = game.entityManager.createEntity("light");
        lightTransform = static_cast<const shared_ptr<Transform>>(lightEntity.transform);
        mat = ResourceManager::GetMaterialInstance("default");
        mat->SetEmission(Vector3::one);
        mat->SetIntensity(intensity);
        lightEntity.AddComponent<Renderer>(nullptr, mat.get());
        component = static_cast<shared_ptr<PointLight>>(&lightEntity.AddComponent<PointLight>(mat.get()));

        // ... & offset in y direction by orbitDist.
        lightTransform->SetParent(entity->transform);
        lightTransform->SetLocalPosition(Vector3(0, 0, 1));

        // orbit every frame.
        game.onUpdate.addListener([&](Game& game){
            Orbit();
        });
    }

    void Light::Orbit() {
        entity->transform->Rotate(Vector3(0 * Time::deltaTime, 0, 0));
    }
}
