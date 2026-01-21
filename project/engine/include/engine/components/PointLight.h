#pragma once

#include "engine/ecs/Component.h"
#include "engine/utilities/Vector3.h"
#include "engine/utilities/rendering/Material.h"

namespace EisEngine{
    namespace rendering { class Shader; }

    namespace components {
        using Component = EisEngine::ecs::Component;
        using Shader = EisEngine::rendering::Shader;

        class PointLight : public Component {
        public:
            explicit PointLight (
                    Game& game, guid_t owner,
                    Material *mat
            );
            PointLight(const PointLight &light) = delete;
            PointLight(PointLight &&other) noexcept;

            void Apply(Shader& shader, const int& index) const;

            Vector3 GetEmission() const { return mat->GetEmission();}
            float GetIntensity() const {return mat->GetIntensity();}
            Vector3 position() const;

            void SetEmission(const Vector3& v) { mat->SetEmission(v);}
            void SetIntensity(const float& I) { mat->SetIntensity(I);}
        private:
            Material* mat;
        };
    }
}
