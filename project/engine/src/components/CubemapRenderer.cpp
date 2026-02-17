#include "engine/components/CubemapRenderer.h"
#include "engine/utilities/Debug.h"
#include "engine/ResourceManager.h"

namespace EisEngine::components {
    CubemapRenderer::CubemapRenderer(EisEngine::Game &engine, EisEngine::ecs::guid_t owner, EisEngine::Cubemap *tex) :
        Component(engine, owner), texture(tex) {
        if(!tex)
            DEBUG_ERROR("No Cubemap Texture attached to cubemap!")
    }

    CubemapRenderer::CubemapRenderer(EisEngine::components::CubemapRenderer &&other) noexcept :
        Component(other) {
        owner = other.owner;
        std::swap(this->texture, other.texture);
    }

    void CubemapRenderer::ApplyData(EisEngine::rendering::Shader &shader) {
        if(!texture)
            return;

        auto cmPos = glGetAttribLocation(shader.GetShaderID(), "cubeMap");
        if(cmPos != -1)
            shader.ApplyCubemap(*texture);
    }

    void CubemapRenderer::Invalidate() {
        Component::Invalidate();
    }
}
