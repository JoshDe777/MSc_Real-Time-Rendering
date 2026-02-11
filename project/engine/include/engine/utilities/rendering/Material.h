#pragma once

#include "engine/utilities/Vector3.h"
#include "engine/utilities/Color.h"

#include <algorithm>

namespace EisEngine {
    namespace rendering{
        class Shader;
    }

    /// \n A class containing an object's material data. Is attached to a Renderer component to apply.
    class Material {
        using Shader = rendering::Shader;
    public:
        /// \n Creates a new Material object from the given data.
        /// @param diffuse - Vector3: The base, lit color/tint for the object.\n
        ///     Defaults to white (1,1,1).
        /// @param emission - Vector3: The color of light emitted by the object in all directions.\n
        ///     Defaults to black (0,0,0) - no light emitted.
        /// @param opacity - float: The object's opacity = inverse transparency.\n
        ///     Defaults to 1 (fully opaque).
        /// @param metallic - float: The degree of metallic property the object has. Correlates with gloss/shininess.\n
        ///     Defaults to 0 (not metallic).
        /// @param roughness - float: The inverse surface uniformity - the rougher the surface, the noisier the lighting effects.\n
        ///     Defaults to 0.5 (semi-matte)
        explicit Material(
                std::string name = "Material",
                const Vector3& diffuse = Vector3(1, 1, 1),
                const Vector3& emission = Vector3::zero,
                const float& opacity = 1,
                const float& metallic = 0,
                const float& roughness = 0.5f
            );

        /// \n Applies material data to the active shader.
        void ApplyMatData(Shader& shader);

        /// \n Prints out the material's values for debugging purposes.
        void Print();

        #pragma region getters
        const Vector3& GetDiffuse() {return diffuse;}
        const Vector3& GetEmission() {return emission;}
        const float& GetOpacity() const {return opacity;}
        const float& GetMetallic() const {return metallic;}
        const float& GetRoughness() const {return roughness;}
        const float& GetTiling() const {return tiling;}
        const float& GetIntensity() const {return intensity;}
        const std::string& Name() {return name;}
        #pragma endregion

        #pragma region setters
        void SetDiffuse(const Vector3& val) {
            auto x = std::clamp(val.x, 0.0f, 1.0f);
            auto y = std::clamp(val.y, 0.0f, 1.0f);
            auto z = std::clamp(val.z, 0.0f, 1.0f);
            diffuse = Vector3(x, y, z);}
        void SetDiffuse(const Color& val) {diffuse = Vector3(val.r, val.g, val.b);}
        void SetEmission(const Vector3& val) {
            auto x = std::clamp(val.x, 0.0f, 1.0f);
            auto y = std::clamp(val.y, 0.0f, 1.0f);
            auto z = std::clamp(val.z, 0.0f, 1.0f);
            emission = Vector3(x, y, z);}
        void SetOpacity(const float& val) {opacity = std::clamp(val, 0.0f, 1.0f);}
        void SetMetallic(const float& val) {metallic = std::clamp(val, 0.0f, 1.0f);}
        void SetRoughness(const float& val) {roughness = std::clamp(val, 0.0f, 1.0f);}
        void SetTiling(const float& val){tiling = val;}
        void SetIntensity(const float& val) {intensity = val;}
        #pragma endregion
    private:
        Vector3 diffuse;
        Vector3 emission;
        float opacity;
        float metallic;
        float roughness;
        /// \n tesselation factor for the texture:
        float tiling = 1.0f;
        float intensity = 0.0f;
        std::string name;
    };
}

using Material = EisEngine::Material;
