#include "engine/components/Renderer.h"
#include "engine/components/PointLight.h"
#include "engine/ResourceManager.h"

namespace EisEngine::components {
    Renderer::Renderer(EisEngine::Game &engine,
                       EisEngine::ecs::guid_t owner,
                       EisEngine::Texture2D *diffTex,
                       EisEngine::Material* mat,
                       std::string layer,
                       Texture2D* normMap) :
            Component(engine, owner),
            diffuseTexture(diffTex),
            material(mat),
            m_layer(std::move(layer)),
            normalMap(normMap) {
        if(!diffTex)
            diffuseTexture = static_cast<shared_ptr<Texture2D>>(ResourceManager::GetTexture("default"));
        if(!mat)
            material = ResourceManager::GetMaterialInstance("default");
        if(!normMap)
            normalMap = static_cast<shared_ptr<Texture2D>>(ResourceManager::GetTexture("default_normal"));
    }

    Renderer::Renderer(EisEngine::components::Renderer &&other) noexcept :
            Component(other) {
        owner = other.owner;
        std::swap(this->diffuseTexture, other.diffuseTexture);
        std::swap(this->material, other.material);
        std::swap(this->m_layer, other.m_layer);
    }

    // applies the selected color to the active shader.
    void Renderer::ApplyData(Shader& shader) {
        material->ApplyMatData(shader);

        if(diffuseTexture != nullptr)
            shader.ApplyTexture2D(*diffuseTexture, DIFFUSE);

        if(normalMap != nullptr)
            shader.ApplyTexture2D(*normalMap, NORMAL);
    }

    void Renderer::Invalidate() {
        Component::Invalidate();
    }
}