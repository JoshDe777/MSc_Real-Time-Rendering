#include <utility>

#include "engine/Game.h"
#include "engine/components/PointLight.h"

#include "engine/ecs/Entity.h"
#include "engine/utilities/rendering/Shader.h"
#include "engine/systems/LightSystem.h"

namespace EisEngine::components {
    PointLight::PointLight(
            Game& game, guid_t owner,
            Material* mat
    ) : Component(game, owner), mat(mat), engine(game) {
        game.lightSystem->InsertEntityAt(owner, entity()->transform->GetGlobalPosition());
    }
    PointLight::PointLight(EisEngine::components::PointLight &&other) noexcept : Component(other), engine(other.engine) {
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

        if(entity()->name() == "Dynamic light")
            DEBUG_LOG("Applied dynamic light!")
    }

    Vector3 PointLight::position() const {
        return entity()->transform->GetGlobalPosition();
    }

    void PointLight::Invalidate() {
        engine.lightSystem->RemoveEntityFromGrid(owner);
        Component::Invalidate();
    }
}
