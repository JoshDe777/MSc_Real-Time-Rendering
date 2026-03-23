#pragma once

#include <OpenGL/OpenGlInclude.h>
#include "engine/ecs/System.h"
#include "Camera.h"
#include "engine/utilities/rendering/Shader.h"

namespace EisEngine{
    namespace components{
        class Mesh2D;
        class Line;
        class Mesh3D;
        class SpriteMesh;
    }
    namespace events{
        template<typename Owner, typename ...Args>
        class Event;
    }
    namespace systems {
        using namespace rendering;
        /// \n The system drawing objects onto the display.
        class RenderingSystem : public System {
            using Mesh3D = EisEngine::components::Mesh3D;
            using Mesh2D = EisEngine::components::Mesh2D;
            using Line = EisEngine::components::Line;
            using Event = EisEngine::events::Event<RenderingSystem, const Vector2&>;
        public:
            /// \n creates an instance of the EisEngine rendering system.
            explicit RenderingSystem(Game& engine);
            /// \n displays meshes of all supported kinds on screen.
            void Draw();

            /// \n Adds an entity as an LOD loader object.
            static void MarkAsLoader(Entity* ptr);
            /// \n Sets the entity to be displayed as the skybox in the scene.
            static void SetSkyboxEntity(Entity* ptr);

            /// \n Switches the shader for 3D objects to the requested shader.
            static void SetActiveShader(const std::string& shaderName);
            /// \n changes the specular factor in the scene for the Blinn-Phong Shader.
            static void SetSpecularFactor(const float& val);
            /// \n sets the amount of brightness levels for the toon shader.
            static void SetToonLevelCount(const int& n) {n_toon_levels = n;}
            /// \n sets the base lighting level for a scene.
            static void SetAmbientLevel(const float& val) { ambient = val;}
            /// \n sets the level of chromatic aberration. Unused outside of glassy shader.
            static void SetEta(const Vector3& val) { eta = val;}
        private:
            // FBO functions
            /// \n Initializes the framebuffer object for depth mapping.
            void InitFBO(const int& index, const Vector2& screenDims);
            /// \n Resizes buffers on window size shift.
            void ResizeFBOItems(const Vector2& newScreenDims);

            // draw sub-functions
            /// \n Draws 2D meshes
            static void DrawMesh2D(Mesh2D& mesh, Shader* activeShader);
            /// \n Draws Lines
            static void DrawLine(Line& mesh, Shader* activeShader);
            /// \n Draws the skybox if it exists
            void DrawSkybox(Shader* activeShader);
            /// \n Prepares shaders to draw a 3D Mesh
            void Prepare3DDraw(Mesh3D& mesh, Shader* activeShader);
            /// \n Drawing program for all translucent objects.
            void DrawTransparentObjects(std::vector<Mesh3D*>& transparentMeshes,
                                        Shader* activeShader);

            // shader stuff
            /// \n VAO array storing a VAO for each type of mesh in order: Mesh2D, Line, Mesh3D, SpriteMesh, uiMesh.
            std::array<GLuint, 5> VAO;
            /// \n FBO array storing a depth-mapping FBO.
            std::array<GLuint, 2> FBO;
            /// \n FBO array storing a depth-mapping RBO.
            std::array<GLuint, 2> RBO;
            /// \n Texture index storing depth data.
            std::array<GLuint, 2> depthTex;
            /// \n The name (human-readable) of the default shader for the rendering system.\n
            /// Must be a key value for a shaderNameDict entry.
            static const std::string defaultShader;
            /// \n The name (human-readable) of the currently active 3D shader for the rendering system.\n
            /// Must be a key value for a shaderNameDict entry.
            static std::string active3DShader;
            /// \n A dictionary linking shader names to the name of their corresponding shader object in the ResourceManager.
            static const std::unordered_map<std::string, std::string> shaderNameDict;

            // composite members
            /// \n A pointer to the active camera object.
            Camera* camera = nullptr;
            /// \n A pointer to the game's light system object.
            LightSystem* lightSystem = nullptr;
            /// \n A list of entities enabling other entities in a certain radius of them to be lit.
            static std::vector<Entity*> Loaders;
            /// \n A pointer to the entity marked as a skybox.
            static Entity* skybox;
            /// \n An event called every time the window resizes.
            static Event onResize;

            // rendering parameters
            /// \n [Blinn-Phong] The specular factor determining how sharp the specular lobe is.\n
            /// The higher this value, the slimmer the lobe.
            static float specularFactor;
            /// \n the amount of brightness levels available to the toon shader.
            static int n_toon_levels;
            /// \n Introduced as a chromatic aberration factor. Unused outside of glassy shader.
            static Vector3 eta;
            /// Proportion of ambient lighting.\n
            /// ambient = 1 -> fully lit scene.
            static float ambient;
        };
    }
}
