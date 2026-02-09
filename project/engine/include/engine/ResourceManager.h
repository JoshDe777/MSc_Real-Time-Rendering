#pragma once

#include "engine/utilities/rendering/Texture2D.h"
#include "engine/utilities/rendering/Shader.h"
#include "engine/utilities/rendering/Material.h"
#include "engine/ecs/Entity.h"

#include <filesystem>
#include <map>
#include <assimp/material.h>
#include <assimp/scene.h>

#define GET_STRING(x) #x
#define GET_DIR(x) GET_STRING(x)

namespace fs = std::filesystem;

namespace EisEngine {
    inline fs::path resolveAssetPath(const fs::path &relativeAssetPath) {
        auto mergedPath = (GET_DIR(ASSET_ROOT) / relativeAssetPath).make_preferred();
        fs::path path;
        try{
            path = fs::canonical(mergedPath);
        }
        catch(exception& e){
            DEBUG_ERROR("Couldn't resolve file " + mergedPath.string())
            return fs::path("Invalid");
        }
        return path;
    }

    /// \n Struct used to debug shader creation.
    struct glStatusData {
        /// \n Indicates whether the shader creation was successful.
        int success;
        /// \n The name given to the shader.
        const char *shaderName;
        /// \n Log messages attributed to the current shader.
        char infoLog[GL_INFO_LOG_LENGTH];
    };

    using Shader = rendering::Shader;

    /// \n Manages files associated with the engine.
    class ResourceManager {
        friend class Game;
    public:
        /// \n Loads a 3D-object (any extension supported by assimp) as a mesh + renderer combination.
        /// @param imagePath - fs::path: the absolute path from the assets folder to the desired file.
        static ecs::Entity* Load3DObject(Game& game, const fs::path& path);

        /// \n Generates a texture from the given file.
        /// @param imagePath - fs::path: the absolute path from the assets folder to the desired file.
        /// @param textureName - std::string: the name of the given texture. Must be unique!
        /// @returns Texture2D*: A pointer to a Texture2D object created from the provided image.
        static Texture2D* GenerateTextureFromFile(const fs::path& imagePath, const std::string& textureName);

        /// \n fetches a texture using its name.
        static Texture2D* GetTexture(const std::string& name);

        /// \n Generates a cubemap texture from 6 images in order right - left - top - bottom - front - back.
        static Cubemap* GenerateCubemapFromFiles(const std::vector<std::string>& imagePaths, const std::string& cubemapName);
        static Cubemap* GetCubemap(const std::string& name);

        /// \n fetches a material using its name.
        static Material* GetMaterial(const std::string& matname);
        /// \n creates an instance of a material using its name.
        static std::unique_ptr<Material> GetMaterialInstance(const std::string& matname);

        /// \n fetches a shader using its name.
        static Shader* GetShader(const std::string& name);

        /// \n Generates a shader program from the given file.
        /// @param vertexShaderPath - std::filesystem::path: the relative path from the assets folder
        /// to the vertex shader definition.
        /// @param fragmentShaderPath - std::filesystem::path: the relative path from the assets folder
        /// to the fragment shader definition.
        /// @param shaderName - std::string: the UNIQUE name for the shader.
        static Shader* GenerateShaderFromFiles(const fs::path& vertexShaderPath,
                                               const fs::path& fragmentShaderPath,
                                               const std::string& shaderName);
    private:
        /// \n Inaccessible constructor. All functions should be used as static members.
        ResourceManager() { }
        /// \n Clears all textures.
        static void Clear();

        /// \n A dictionary of textures associated to their file name.
        static std::map<std::string, std::unique_ptr<Texture2D>> Textures;
        /// \n A dictionary of textures associated to their file name.
        static std::map<std::string, std::unique_ptr<Cubemap>> Cubemaps;
        /// \n A dictionary of shaders associated to their name.
        static std::map<std::string, std::unique_ptr<Shader>> Shaders;
        /// \n A dictionary of materials associated to their name.
        static std::map<std::string, std::unique_ptr<Material>> Materials;

        /// \n Compiles a file path to a std::string.
        static std::string ReadText(const fs::path& path);
        /// \n Imports data from a given data node in an assimp scene.
        /// \n Creates an entity per submesh, storing mesh, texture and material data in said entity.
        /// Said entity is then attached to the parent entity.
        /// @param game - Game&: A reference to the Engine system.
        /// @param node - aiNode*: A pointer to the current node from which the data should be imported.
        /// @param scene - aiScene*: A pointer to the overall scene graph the node is from.
        /// @param parent - Entity*: A pointer to the parent object to this node.
        static void ImportNode(Game& game, const aiNode* node,
                               const aiScene* scene, const fs::path& modelPath, Entity* parent = nullptr);

        #pragma region Textures
        /// \n loads a texture from a file.
        /// @param filePath - std::filesystem::path: The file path to the image.
        /// @param alpha - bool: Determines whether the texture has an alpha channel or not.
        static Texture2D loadTextureFromFile(const fs::path& filePath);

        /// \n loads a texture from an assimp scene.
        static Texture2D* ImportTextureFromAssimp(const aiMaterial* mat, const aiScene* scene,
                                                  const fs::path& modelPath);

        /// \n creates a dummy, white texture.
        static Texture2D* MakeDummyTexture();
        /// \n creates a dummy, blue texture.
        static Texture2D* MakeDummyNormalMap();
        #pragma endregion

        #pragma region Cubemaps
        static Cubemap loadCubemapFromFiles(const std::vector<std::string>& filePaths);
        #pragma endregion

        #pragma region Materials
        /// \n Loads a material from the provided assimp material object.
        static Material* LoadMaterial(const aiMaterial* mat);
        #pragma endregion

        #pragma region Shaders
        /// \n Creates a shader program of the given shaderType with a definition at the given shader path.
        static unsigned int loadAndCompileShader(GLuint shaderType, const fs::path& filePath);
        #pragma endregion
    };
}
