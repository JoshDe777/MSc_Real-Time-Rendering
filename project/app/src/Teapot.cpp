#include "Teapot.h"

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

    Teapot::Teapot(Game& game) {
        auto temp = ResourceManager::Load3DObject(game, fs::path("3d-objects/cube.fbx"));
        temp->transform->SetLocalPosition(Vector3(0, 0, 0));
        //temp->transform->PrintRelativeSceneGraph();

        if(temp->GetComponent<Mesh3D>() == nullptr && !temp->transform->getChildren().empty()){
            for(auto child: temp->transform->getChildren()){
                if(child->entity()->GetComponent<Mesh3D>() != nullptr){
                    temp = child->entity();
                    goto meshEntity;
                }
                for(auto grandchild: child->getChildren()){
                    if(grandchild->entity()->GetComponent<Mesh3D>() != nullptr){
                        temp = grandchild->entity();
                        goto meshEntity;
                    }
                }
            }
            DEBUG_RUNTIME_ERROR("Couldn't find an entity in the scene graph with a mesh.")
        }

    meshEntity:
        entity = static_cast<shared_ptr<Entity>>(temp);
        if(entity->GetComponent<Renderer>() != nullptr)
            entity->RemoveComponent<Renderer>();

        auto diffTex = ResourceManager::GenerateTextureFromFile(diffuseTexPath, "Brick_Wall_Diffuse");
        auto normTex = ResourceManager::GenerateTextureFromFile(normalTexPath, "Brick_Wall_Normal");

        renderer = static_cast<shared_ptr<Renderer>>(&entity->AddComponent<Renderer>(
                diffTex,
                nullptr,
                "none",
                normTex));

        temp->transform->SetLocalRotation(GetRandomOrbitAxis());
        entity->transform->SetLocalPosition(Vector3::zero);

        renderer->material->SetDiffuse(Color::white);
        renderer->material->SetMetallic(1.0f);
        renderer->material->SetRoughness(0.7f);
        renderer->material->SetOpacity(1.0f);

        game.onUpdate.addListener([&](Game& game){
            rotate();
        });
    }

    void Teapot::setRoughness(const float &val) {
        auto endval = std::clamp(val, 0.0f, 1.0f);
        renderer->material->SetRoughness(endval);
    }

    void Teapot::setOpacity(const float &val) {
        auto endval = std::clamp(val, 0.0f, 1.0f);
        renderer->material->SetOpacity(endval);
    }

    void Teapot::rotate() {
        //entity->transform->Rotate(Vector3(0, 0, rotationSpeed * Time::deltaTime));
    }
}