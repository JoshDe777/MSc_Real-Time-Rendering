#pragma once

#include <OpenGL/OpenGlInclude.h>
#include "engine/ecs/System.h"
#include "Camera.h"
#include "engine/utilities/rendering/Shader.h"

namespace EisEngine{
    namespace components{
        class Mesh3D;
    }
    namespace events{
        template<typename Owner, typename ...Args>
        class Event;
    }
    namespace systems {
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
            using Mesh3D = EisEngine::components::Mesh3D;
            using Event = EisEngine::events::Event<RenderingSystem, const Vector2&>;
        public:
            /// \n creates an instance of the EisEngine rendering system.
            explicit RenderingSystem(Game& engine);
            /// \n displays meshes of all supported kinds on screen.
            void Draw();
            /// \n Adds an entity as an LOD loader object.
            static void MarkAsLoader(Entity* ptr);
            /// \n changes the specular factor in the scene for the Blinn-Phong Shader.
            static void SetSpecularFactor(const float& val);
            /// \n Switches the shader for 3D objects to the requested shader.
            static void SetActiveShader(const std::string& shaderName);
            /// \n sets the amount of brightness levels for the toon shader.
            static void SetToonLevelCount(const int& n) {n_toon_levels = n;}
        private:
            /// \n Draws a 3D Mesh
            void PrepareDraw(Mesh3D& mesh, Shader* activeShader);
            /// \n Initializes the framebuffer object for depth mapping.
            void InitFBO(const int& index, const Vector2& screenDims);
            /// \n Resizes buffers on window size shift.
            void ResizeFBOItems(const Vector2& newScreenDims);

            /// \n A pointer to the active camera object.
            Camera* camera = nullptr;
            /// \n VAO array storing a VAO for each type of mesh in order: Mesh2D, Line, Mesh3D, SpriteMesh, uiMesh.
            std::array<GLuint, 5> VAO;
            /// \n FBO array storing a depth-mapping FBO.
            std::array<GLuint, 2> FBO;
            /// \n FBO array storing a depth-mapping RBO.
            std::array<GLuint, 2> RBO;
            /// \n Texture index storing depth data.
            std::array<GLuint, 2> depthTex;
            /// \n An event called every time the window resizes.
            static Event onResize;
            /// \n A list of entities enabling other entities in a certain radius of them to be lit.
            static std::vector<Entity*> Loaders;
            /// \n A reference of Point Lights by approximate position in world 2D (x, z) space.
            std::unordered_map<Vector2, std::vector<int>, GridCoordHashMap> LightGrid = {};
            /// \n Collects all Point Lights in the scene and compiles them to a usable grid.
            void BuildLightGrid();
            /// \n Checks the surroundings of an object for effecting light sources.
            std::vector<int> QueryNearbyLights(const glm::vec3& objectPos);
            /// \n [Blinn-Phong] The specular factor determining how sharp the specular lobe is.\n
            /// The higher this value, the slimmer the lobe.
            static float specularFactor;
            /// \n The name (human-readable) of the default shader for the rendering system.\n
            /// Must be a key value for a shaderNameDict entry.
            static const std::string defaultShader;
            /// \n The name (human-readable) of the currently active 3D shader for the rendering system.\n
            /// Must be a key value for a shaderNameDict entry.
            static std::string active3DShader;
            /// \n A dictionary linking shader names to the name of their corresponding shader object in the ResourceManager.
            static const std::unordered_map<std::string, std::string> shaderNameDict;
            /// \n the amount of brightness levels available to the toon shader.
            static int n_toon_levels;
        };
    }
}
