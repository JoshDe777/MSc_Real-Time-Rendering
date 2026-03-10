#include "Torch.h"

namespace Maze {

    Torch::Torch(EisEngine::Game &game) {
        entity = static_cast<const shared_ptr<Entity>>(ResourceManager::Load3DObject(game, "3d-objects/own_torch.fbx"));
        entity->transform->SetLocalScale(Vector3(0.0003f, 0.0003f, 0.0003f));
        entity->transform->SetLocalRotation(Vector3(-90, 90, 0));
        std::vector<shared_ptr<Transform>> unexplored = {entity->transform};
        while(flame == nullptr && !unexplored.empty()){
            auto current = unexplored[0];
            unexplored.erase(unexplored.begin());

            auto name = current->entity()->name();
            if(name == "Flame"){
                flame = static_cast<shared_ptr<Renderer>>(current->entity()->GetComponent<Renderer>());
                break;
            }

            for(auto child : current->getChildren()){
                unexplored.push_back(child);
            }
        }
        unexplored.clear();

        noise = make_unique<FastNoiseLite>();
        noise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        auto perlinSeed = RandomInt(0, std::numeric_limits<int>::max());
        noise->SetSeed(perlinSeed);

        game.onUpdate.addListener([&](Game& game){
            Flicker();
        });
    }

    void Torch::Flicker() {
        auto ival = 0.5f * noise->GetNoise(0.0f, animTime * flickerSpeed) + 0.55f;
        flame->material->SetIntensity(ival * maxIntensity);
        animTime = animTime + flickerSpeed * Time::deltaTime;
    }
}
