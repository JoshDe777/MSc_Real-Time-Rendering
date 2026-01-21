#include "engine/components/Renderer.h"
#include "engine/components/PointLight.h"
#include "engine/ResourceManager.h"

namespace EisEngine::components {
    Renderer::Renderer(EisEngine::Game &engine,
                       EisEngine::ecs::guid_t owner,
                       EisEngine::Texture2D *tex,
                       EisEngine::Material* mat,
                       std::string layer) :
                       Component(engine, owner),
                       texture(tex),
                       material(mat),
                       m_layer(std::move(layer)) {
        if(!tex)
            texture = ResourceManager::GetTexture("default");
        if(!mat)
            material = ResourceManager::GetMaterialInstance("default");
    }

    Renderer::Renderer(EisEngine::components::Renderer &&other) noexcept :
            Component(other) {
        owner = other.owner;
        std::swap(this->texture, other.texture);
        std::swap(this->material, other.material);
        std::swap(this->m_layer, other.m_layer);
    }

    // applies the selected color to the active shader.
    void Renderer::ApplyData(Shader& shader) {
        material->ApplyMatData(shader);

        if(!texture)
            return;

        shader.ApplyTexture(*texture);
    }

    void Renderer::Invalidate() {
        Component::Invalidate();
    }
}