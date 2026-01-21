#include "engine/components/PointLight.h"

#include "engine/ecs/Entity.h"
#include "engine/utilities/rendering/Shader.h"

namespace EisEngine::components {
    PointLight::PointLight(
            Game& game, guid_t owner,
            Material *mat
    ) : Component(game, owner), mat(mat) { }
    PointLight::PointLight(EisEngine::components::PointLight &&other) noexcept : Component(other) {
        owner = other.owner;
        std::swap(this->mat, other.mat);
    }

    void PointLight::Apply(rendering::Shader &shader, const int& index) const {
        std::stringstream loc;
        loc.str("");
        loc << "lights[" << index << "].emission";
        shader.setVector(loc.str().c_str(), GetEmission());
        loc.str("");
        loc << "lights[" << index << "].pos";
        shader.setVector(loc.str().c_str(), position());
        loc.str("");
        loc << "lights[" << index << "].I";
        shader.setFloat(loc.str().c_str(), GetIntensity());
    }

    Vector3 PointLight::position() const {
        return entity()->transform->GetGlobalPosition();
    }
}
