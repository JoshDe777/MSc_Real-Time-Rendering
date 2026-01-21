#include <utility>

#include "engine/utilities/rendering/Material.h"
#include "engine/utilities/rendering/Shader.h"
#include "engine/utilities/Debug.h"

namespace EisEngine {
    Material::Material(std::string  name, const Vector3& diffuse, const Vector3 &emission,
                       const float &opacity, const float &metallic,
                       const float &roughness) :
                       name(std::move(name)), diffuse(diffuse), emission(emission),
                       opacity(opacity), metallic(metallic), roughness(roughness) {}

    void Material::ApplyMatData(Shader &shader) {
        shader.setVector("diffuse", diffuse);
        shader.setFloat("alpha", opacity);
        shader.setFloat("tiling", tiling);
        shader.setFloat("shiny", metallic * (1 - roughness));
    }

    void Material::Print() {
        DEBUG_INFO("Material \"" + name + "\" properties:\nDiffuse (vec3)=" + (std::string)diffuse +
        "\nEmission (vec3)=" + (std::string)emission +
        "\nOpacity (float)=" + std::to_string(opacity) +
        "\nMetallic (float)=" + std::to_string(metallic) +
        "\nRoughness (float)=" + std::to_string(roughness))
    }
}
