#include "engine/components/Renderer.h"
#include "engine/components/PointLight.h"
#include "engine/ResourceManager.h"

namespace EisEngine::components {
    Renderer::Renderer(EisEngine::Game &engine,
                       EisEngine::ecs::guid_t owner,
                       const shared_ptr<EisEngine::Texture2D>& diffTex,
                       const shared_ptr<EisEngine::Material>& mat,
                       std::string layer,
                       const shared_ptr<Texture2D>& normMap) :
            Component(engine, owner),
            diffuseTexture(diffTex),
            material(mat),
            m_layer(std::move(layer)),
            normalMap(normMap) {
        if(!diffTex)
            diffuseTexture = ResourceManager::GetTexture("default");
        if(!mat)
            material = ResourceManager::GetMaterialInstance("default");
        if(!normMap)
            normalMap = ResourceManager::GetTexture("default_normal");
    }

    Renderer::Renderer(EisEngine::components::Renderer &&other) noexcept :
            Component(other),
            diffuseTexture(std::move(other.diffuseTexture)),
            normalMap(std::move(other.normalMap)),
            material(std::move(other.material)),
            m_layer(std::move(other.m_layer)){
        owner = other.owner;
        other.owner = -1;
    }

    // applies the selected color to the active shader.
    void Renderer::ApplyData(Shader& shader) {
        material->ApplyMatData(shader);

        if(diffuseTexture != nullptr){
            auto diffPos = glGetUniformLocation(shader.GetShaderID(), "image");
            if(diffPos != -1) {
                shader.ApplyTexture2D(*diffuseTexture, DIFFUSE);
            }
        }


        if(normalMap != nullptr) {
            auto nPos = glGetUniformLocation(shader.GetShaderID(), "nMap");
            if(nPos != -1) {
                shader.ApplyTexture2D(*normalMap, NORMAL);
                DEBUG_INFO(entity()->name() + " normal map applied.")
            }
        }
    }

    void Renderer::Invalidate() {
        diffuseTexture.reset();
        normalMap.reset();
        material.reset();
        Component::Invalidate();
    }
}