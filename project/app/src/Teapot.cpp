#include "Teapot.h"

namespace RTR {
    Teapot::Teapot(Game& game, const int& index) {
        entity = std::make_shared<Entity>(game.entityManager.createEntity("Checkerboard_Sprite_" + std::to_string(index)));
        entity->transform->SetLocalPosition(Vector3(0, 0, 0));
        //temp->transform->PrintRelativeSceneGraph();
        entity->AddComponent<SpriteMesh>();


        texture = static_cast<shared_ptr<Texture2D>>(ResourceManager::GenerateTextureFromFile(texPath, "Checkerboard_" + std::to_string(index)));

        renderer = static_cast<shared_ptr<Renderer>>(&entity->AddComponent<Renderer>(texture.get()));

        entity->transform->SetLocalPosition(Vector3::zero);

        renderer->material->SetDiffuse(Color::white);
        renderer->material->SetMetallic(1.0f);
        renderer->material->SetRoughness(1.0f);
        renderer->material->SetOpacity(1.0f);
    }

    void Teapot::setRoughness(const float &val) {
        auto endval = std::clamp(val, 0.0f, 1.0f);
        renderer->material->SetRoughness(endval);
    }

    void Teapot::setOpacity(const float &val) {
        auto endval = std::clamp(val, 0.0f, 1.0f);
        renderer->material->SetOpacity(endval);
    }

    void Teapot::setFilterMode(const int &val) {
        texture->SetFilteringMode(val <= 3 ? (FilterModes) val : MIPMAP_LINEAR);
    }
}