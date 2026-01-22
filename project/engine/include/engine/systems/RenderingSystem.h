#pragma once

#include <OpenGL/OpenGlInclude.h>
#include "engine/ecs/System.h"
#include "Camera.h"
#include "engine/utilities/rendering/Shader.h"

namespace EisEngine::systems {
    struct GridCoordHashMap{
        size_t operator()(const Vector2& c) const {
            // good 2D hash — no collisions for small/medium worlds
            return (std::hash<int>()(c.x) * 73856093) ^
                   (std::hash<int>()(c.y) * 19349663);
        }
    };

    using namespace rendering;
    /// \n The system drawing objects onto the display.
    class RenderingSystem : public System {
    public:
        /// \n creates an instance of the EisEngine rendering system.
        explicit RenderingSystem(Game& engine);
        /// \n displays meshes of all supported kinds on screen.
        void Draw();
        /// \n Adds an entity as an LOD loader object.
        static void MarkAsLoader(Entity* ptr);
        /// \n changes the specular factor in the scene for the Blinn-Phong Shader.
        static void SetSpecularFactor(const float& val);
    private:
        /// \n A pointer to the active camera object.
        Camera* camera = nullptr;
        /// \n VAO array storing a VAO for each type of mesh in order: Mesh2D, Line, Mesh3D, SpriteMesh, uiMesh.
        std::array<GLuint, 5> VAO;
        GLuint FBO;
        static std::vector<Entity*> Loaders;
        std::unordered_map<Vector2, std::vector<int>, GridCoordHashMap> LightGrid = {};
        void BuildLightGrid();
        std::vector<int> QueryNearbyLights(const glm::vec3& objectPos);
        static float specularFactor;
    };
}
