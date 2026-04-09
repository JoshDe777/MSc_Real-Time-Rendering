#include <utility>

#include "engine/utilities/rendering/Material.h"
#include "engine/utilities/rendering/Shader.h"
#include "engine/utilities/Debug.h"
#include "engine/utilities/Math.h"
#include "engine/utilities/Vector3.h"

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
        shader.setFloat("metallic", metallic);
        shader.setFloat("roughness", roughness);
    }

    void Material::Print() {
        DEBUG_INFO("Material \"" + name + "\" properties:\nDiffuse (vec3)=" + (std::string)diffuse +
        "\nEmission (vec3)=" + (std::string)emission +
        "\nOpacity (float)=" + std::to_string(opacity) +
        "\nMetallic (float)=" + std::to_string(metallic) +
        "\nRoughness (float)=" + std::to_string(roughness))
    }

    Vector3 Material::eval(const EisEngine::Vector3 &normal,
                        const EisEngine::Vector3 &dirToLight,
                        const EisEngine::Vector3 &view) {
        // phong specular lobe pre-calculation (no changes to diffuse for simplicity)
        auto ref_factor = (2 * Vector3::DotProduct(normal, dirToLight) * (normal - dirToLight)).normalized();
        auto ref_cos = max(Vector3::DotProduct(ref_factor, view), 0);
        float shiny = (1-roughness);

        auto specular = diffuse * pow(ref_cos, shiny);

        return diffuse + specular;
    }
}
