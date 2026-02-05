#pragma once

#include "engine/ecs/Component.h"
#include "engine/utilities/rendering/Shader.h"

namespace EisEngine {
    using ecs::Component;
    using rendering::Shader;

    namespace components {

        class CubemapRenderer: public Component {
        public:
            CubemapRenderer(Game& engine, guid_t owner, Cubemap* tex);
            CubemapRenderer(const CubemapRenderer &renderer) = delete;
            CubemapRenderer(CubemapRenderer &&other) noexcept;

            virtual void ApplyData(Shader& shader);
            void SetCubemapTexture(Cubemap* newTex) {texture = newTex;}
            Cubemap* GetTexture() {return texture;}
        protected:
            void Invalidate() override;
            Cubemap* texture;
        };
    }
} // EisEngine
