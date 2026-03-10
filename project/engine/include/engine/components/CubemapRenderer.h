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
            void SetCubemapTexture(Cubemap* newTex) {texture = static_cast<shared_ptr<Cubemap>>(newTex);}
            Cubemap* GetTexture() {return texture.get();}
        protected:
            void Invalidate() override;
            shared_ptr<Cubemap> texture;
        };
    }
} // EisEngine
