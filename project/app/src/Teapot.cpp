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

        auto tempEntity = ResourceManager::Load3DObject(game,"3d-objects/utah_teapot.obj");
        tempEntity->transform->SetLocalRotation(GetRandomOrbitAxis());

        // this isn't working. Try debugging the Load3DObject sequence again and find the correct depth of object referencing!
        for(auto child : tempEntity->transform->getChildren()){
            auto temp = child->entity();
            auto tempRenderer = temp->GetComponent<Renderer>();
            if(tempRenderer)
                entity = static_cast<shared_ptr<Entity>>(temp);
            else
                for(auto grandchild : child->getChildren()){
                    temp = grandchild->entity();
                    tempRenderer = temp->GetComponent<Renderer>();
                    if(!tempRenderer)
                        DEBUG_RUNTIME_ERROR("No Renderers found on object.")
                    else
                        entity = static_cast<shared_ptr<Entity>>(temp);
                }
        }

        auto temp = entity->GetComponent<Renderer>();
        if(!temp)
            DEBUG_RUNTIME_ERROR("Couldn't find Renderer on Teapot!")

        renderer = static_cast<const shared_ptr<Renderer>>(temp);
        renderer->material->SetDiffuse(Color::red);
        renderer->material->SetMetallic(0.7f);
        renderer->material->Print();

        game.onUpdate.addListener([&](Game& game){
            rotate();
        });
    }

    void Teapot::setShininess(const float &val) {
        auto endval = std::clamp(val, 0.0f, 1.0f);
        renderer->material->SetMetallic(endval);
    }

    void Teapot::rotate() {
        entity->transform->Rotate(Vector3(0, 0, rotationSpeed * Time::deltaTime));
    }
}