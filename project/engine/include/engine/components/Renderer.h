#pragma once

#include "engine/ecs/Component.h"
#include "engine/utilities/rendering/Shader.h"
#include "engine/utilities/rendering/Material.h"
#include "engine/utilities/rendering/Texture2D.h"


namespace EisEngine {
    using ecs::Component;
    using rendering::Shader;

    namespace components {
        /// \n *deprecated* Contains rendering data for any type of mesh.
        /// \n Use specialized renderers tailored to a specific mesh type instead.
        class Renderer : public Component {
        public:
            /// \n Creates a renderer.
            /// @param diffTex - Texture2D*: A pointer to a texture item. Can be null.
            /// @param mat - Material*: A pointer to a material item.
            /// @param layer - std::string: The rendering layer for meshes paired with a renderer.
            /// \n Currently supported: {"UI" for UI Elements, and [any other string] for regular rendering}.
            Renderer(Game &engine, guid_t owner,
                     Texture2D* diffTex = nullptr,
                     Material* mat = nullptr,
                     std::string  layer = "Background",
                     Texture2D* normMap = nullptr);
            Renderer(const Renderer &renderer) = delete;
            Renderer(Renderer &&other) noexcept;

            /// \n Applies rendering data to the active shader before drawing meshes.
            virtual void ApplyData(Shader& shader);
            /// \n returns a renderer's rendering layer.
            [[nodiscard]] std::string GetLayer() { return m_layer;}

            /// \n Sets a new texture for the corresponding sprite.
            void SetDiffuseTexture(Texture2D* newTexture) { diffuseTexture = static_cast<shared_ptr<Texture2D>>(newTexture);}
            void SetNormalMap(Texture2D* newTexture) { normalMap = static_cast<shared_ptr<Texture2D>>(newTexture);}
            /// \n Returns a pointer to the texture assigned to a renderer.
            Texture2D* GetDiffuseTexture() { return diffuseTexture.get();}
            Texture2D* GetNormalMap(){ return normalMap.get();}

            /// \n The material attributed to the associated mesh.
            shared_ptr<Material> material;
        protected:
            /// \n A function called when a component is intentionally deleted.
            void Invalidate() override;
            /// \n the diffuse/albedo texture attributed to the associated mesh.
            shared_ptr<Texture2D> diffuseTexture;
            /// \n the normal map attributed to the associated mesh.
            shared_ptr<Texture2D> normalMap;
            /// \n The rendering layer of the associated mesh.
            std::string m_layer;
        };
    }
}
