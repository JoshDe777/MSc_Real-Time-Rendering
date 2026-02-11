#include "engine/Game.h"
#include "engine/ResourceManager.h"
#include "engine/Components.h"
#include "engine/utilities/Debug.h"

#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace EisEngine {
    std::map<std::string, std::unique_ptr<Texture2D>> ResourceManager::Textures = {};
    std::map<std::string, std::unique_ptr<Cubemap>> ResourceManager::Cubemaps = {};
    std::map<std::string, std::unique_ptr<Material>> ResourceManager::Materials = {};
    std::map<std::string, std::unique_ptr<Shader>> ResourceManager::Shaders = {};
    Assimp::Importer importer;

    Vector3 GetAveragePos(const std::vector<Vector3>& v){
        Vector3 res = Vector3();
        float modifier = (float) 1 / v.size();
        for(auto vx : v)
            res += vx * modifier;

        return res;
    }

#pragma region 3D asset import
    /// \n Imports mesh data (vertices, normals, indices and UVs) from an assimp mesh.
    PrimitiveMesh3D ImportMesh(const aiMesh* mesh){
        auto nVerts = mesh->mNumVertices;
        // vertex collection -> take aiMesh's array of vertices and convert to own format of Vec3's
        std::vector<Vector3> vertices(mesh->mVertices, mesh->mVertices + nVerts);

        // index collection -> iterate through faces & insert the indices for each triangle.
        std::vector<unsigned int> indices = {};
        for(auto i = 0; i < mesh->mNumFaces; i++){
            const auto& face = mesh->mFaces[i];

            // throw an error if not a triangular face.
            assert(face.mNumIndices == 3);
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }

        // normals collection -> same process as vertex collection
        // (mNormals is always of length mNumVertices, hence the use).
        std::vector<Vector3> normals(mesh->mNormals, mesh->mNormals + nVerts);

        // UV map collection - only collecting from the first channel.
        std::vector<Vector2> uvs;
        uvs.reserve(nVerts);
        if(mesh->HasTextureCoords(0)){
            for(auto i = 0; i < nVerts; i++){
                const auto& uv = mesh->mTextureCoords[0][i];
                uvs.emplace_back(uv);
            }
        }
        else    // if no built-in UVs, give each vertex a texture coord of (0, 0)
            uvs.assign(nVerts, Vector2(0, 0));

        std::vector<Vector3> tans;
        tans.reserve(nVerts);
        for(auto i = 0; i < nVerts; i++){
            const auto& tan = mesh->mTangents[i];
            tans.emplace_back(tan);
        }

        std::vector<Vector3> bitans;
        bitans.reserve(nVerts);
        for(auto i = 0; i < nVerts; i++){
            const auto& bitan = mesh->mBitangents[i];
            bitans.emplace_back(bitan);
        }

        return PrimitiveMesh3D(
                vertices,
                indices,
                &normals,
                &uvs,
                &tans,
                &bitans
            );
    }

    void ResourceManager::ImportNode(Game& game, const aiNode* node,
                                     const aiScene* scene, const fs::path& modelPath, Entity* parent){
        // if no meshes or children, return
        if(node->mNumMeshes == 0 && node->mNumChildren == 0)
            return;

        //DEBUG_INFO("Processing entity " + (std::string) node->mName.C_Str() + ".")

        // Create entity for node & attach to parent if exists.
        auto nodeEntity = game.entityManager.createEntity(node->mName.C_Str());
        if(parent){
            nodeEntity.transform->SetParent(parent->transform);
        }

        // get transform data & update entity transform
        aiVector3D scale, pos;
        aiQuaternion rotation;
        node->mTransformation.Decompose(scale, rotation, pos);
        nodeEntity.transform->SetLocalScale(Vector3(scale));
        Vector3 eulerRotation = Vector3(glm::eulerAngles(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z)));
        nodeEntity.transform->SetLocalRotation(eulerRotation);
        nodeEntity.transform->SetLocalPosition(Vector3(pos));

        // foreach mesh in node->nMeshes
        for(unsigned int i = 0; i < node->mNumMeshes; i++){

            auto index = node->mMeshes[i];
            auto mesh = scene->mMeshes[index];

            // Safety check — skip non-triangular or non-vertex meshes
            if (!mesh->HasPositions() || mesh->mNumVertices == 0)
                continue;

            // get mesh data as primitiveMesh
            auto primitive = ImportMesh(mesh);

            // get texture & material data
            auto assimpMaterial = scene->mMaterials[mesh->mMaterialIndex];
            Material* mat = LoadMaterial(assimpMaterial);
            auto tex = ImportTextureFromAssimp(assimpMaterial, scene, modelPath);

            // add Mesh3D & Renderer components
            nodeEntity.AddComponent<Mesh3D>(primitive);
            nodeEntity.AddComponent<Renderer>(tex, mat, "");
            if(mat->GetEmission() != Vector3::zero)
                nodeEntity.AddComponent<PointLight>(mat);
        }

        // import all child nodes recursively
        for(unsigned int i = 0; i < node->mNumChildren; i++)
            ImportNode(game, node->mChildren[i], scene, modelPath, &nodeEntity);
    }

    ecs::Entity* ResourceManager::Load3DObject(Game& game, const fs::path &path) {
        auto fullPath = resolveAssetPath(path);
        std::string pathString = fullPath.string();
        // import the asset with a few optimizations for efficiency:
        // meshes triangulated & optimized, normals generated if not exist, and tangents calculated for normals.
        const aiScene* scene = importer.ReadFile(
                pathString.c_str(),
                aiProcess_Triangulate |
                aiProcess_GenNormals |
                aiProcess_OptimizeMeshes |
                aiProcess_JoinIdenticalVertices |
                aiProcess_CalcTangentSpace
            );

        // exit with an error message if scene loading failed
        // (scene = nullptr, scene flagged incomplete, or no root node).
        if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
            DEBUG_ERROR("Assimp Error: " + std::string(importer.GetErrorString()))
            return nullptr;
        }

        // recursively import data following the aiScene graph.
        auto& rootEntity = game.entityManager.createEntity(path.filename().string());
        ImportNode(game, scene->mRootNode, scene, path.parent_path(), &rootEntity);

        // return the resulting entity.
        return &rootEntity;
    }
#pragma endregion

#pragma region Material handling

    /// \n Imports material data from an assimp material.
    Material* ResourceManager::LoadMaterial(const aiMaterial* mat){
        auto matName = std::string(mat->GetName().C_Str());
        if(Materials[matName] == nullptr){
            Materials[matName] = make_unique<Material>(matName);
            auto result = Materials[matName].get();

            // get properties:
            // -diffuse color AI_MATKEY_COLOR_DIFFUSE
            aiColor4D diffuse;
            aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
            result->SetDiffuse(Vector3(diffuse.r, diffuse.g, diffuse.b));

            // -emission color AI_MATKEY_COLOR_EMISSIVE
            aiColor4D emissive;
            aiGetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE, &emissive);
            result->SetEmission(Vector3(emissive.r, emissive.g, emissive.b));

            // assign default emission if missing.
            if(!emissive.IsBlack())
                result->SetIntensity(1.0f);

            // -opacity AI_MATKEY_OPACITY
            float opacity;
            mat->Get(AI_MATKEY_OPACITY, opacity);
            result->SetOpacity(opacity);

            // -roughness: no matkey - non-phong attributes obtained with $raw or $mat keys.
            auto roughness = 0.5f;
            if(mat->Get("$raw.Roughness", 0, 0, roughness) != AI_SUCCESS)
                mat->Get("$mat.roughnessFactor", 0, 0, roughness);
            result->SetRoughness(roughness);

            // -metallic: no matkey
            auto metallic = 0.0f;
            if(mat->Get("$raw.Metalness", 0, 0, metallic) != AI_SUCCESS)
                mat->Get("$mat.metallicFactor", 0, 0, metallic);
            result->SetMetallic(metallic);
        }
        /*else
            DEBUG_WARN("Attempting to overwrite existing material " + matName + ".")*/
        return Materials[matName].get();
    }

    Material *ResourceManager::GetMaterial(const std::string &matname) {
        // always have a default texture at the ready
        if(matname == "default" && !Materials["default"].get())
            Materials["default"] = std::make_unique<Material>(Material("default"));

        if(Materials.empty()){
            DEBUG_WARN("No textures created in resource manager system.")
            return nullptr;
        }

        return Materials[matname].get();
    }

    std::unique_ptr<Material> ResourceManager::GetMaterialInstance(const std::string &matname) {
        // always have a default texture at the ready
        if(matname == "default" && !Materials["default"].get())
            Materials["default"] = std::make_unique<Material>(Material("default"));

        if(Materials.empty()){
            DEBUG_WARN("No textures created in resource manager system.")
            return nullptr;
        }

        return std::make_unique<Material>(*Materials[matname].get());
    }
#pragma endregion

#pragma region Texture handling
    /// \n Imports the texture from a given material.
    Texture2D* ResourceManager::ImportTextureFromAssimp(
            const aiMaterial* mat, const aiScene* scene, const fs::path& modelPath) {
        const aiTexture *tex = nullptr;

        // get texture from material. Only embedded textures supported because yeah.
        aiString path;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
        // check for embedded texture & error out if not.
        if (path.C_Str()[0] == '*')
            tex = scene->GetEmbeddedTexture(path.C_Str());
        else if (path.length == 0)
            return GetTexture("default");
        else {
            auto texPath = fs::path(modelPath.string() + "\\" + path.C_Str());
            DEBUG_INFO(texPath.string())
            return GetTexture("default"); // GenerateTextureFromFile(texPath, path.C_Str());
        }
        auto textureName = std::string(tex->mFilename.C_Str());

        // make sure no duplicate textures.
        if(Textures[textureName] == nullptr){
            // reformat aiTexture data to usable formats.
            auto* dataInCharPtr = reinterpret_cast<unsigned char*>(tex->pcData);
            // stb_image process just like loadFromFile.
            int width, height, nrChannels;
            auto* data = stbi_load_from_memory(dataInCharPtr, tex->mWidth, &width, &height,
                                               &nrChannels, STBI_rgb_alpha);
            if(!data){
                DEBUG_ERROR("Failed to load texture: " + textureName)
                return nullptr;
            }

            auto texture = Texture2D();

            if(nrChannels == 4) {
                texture.internalFormat = GL_RGBA;
                texture.imageFormat = GL_RGBA;
            }

            texture.Generate(width, height, data);
            stbi_image_free(data);

            // Add to texture registry.
            Textures[textureName] = make_unique<Texture2D>(texture);
        }
        else
            DEBUG_WARN("Attempting to overwrite existing texture " + textureName + ".")
        return GetTexture(textureName);
    }

    Texture2D* ResourceManager::GenerateTextureFromFile( const fs::path &imagePath, const std::string &textureName) {
        if (Textures[textureName] == nullptr)
            Textures[textureName] = std::make_unique<Texture2D>(
                    loadTextureFromFile(resolveAssetPath(imagePath)));
        return GetTexture(textureName);
    }

    Texture2D ResourceManager::loadTextureFromFile(const fs::path& filePath) {
        stbi_set_flip_vertically_on_load(1);

        std::string pathString = filePath.string();
        const char* filename = pathString.c_str();
        Texture2D texture;

        int width, height, nrChannels;
        unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

        if(!data){
            DEBUG_ERROR("Failed to load image: " + pathString)
            return texture;
        }

        if(nrChannels == 4) {
            texture.internalFormat = GL_RGBA;
            texture.imageFormat = GL_RGBA;
        }

        texture.Generate(width, height, data);

        stbi_image_free(data);

        return texture;
    }

    Texture2D *ResourceManager::MakeDummyTexture() {
        if(Textures["default"] == nullptr){
            Texture2D texture;

            int width = 1;
            int height = 1;
            unsigned char data[4] = {255, 255, 255, 255};

            texture.internalFormat = GL_RGBA;
            texture.imageFormat = GL_RGBA;

            texture.Generate(width, height, data);
            Textures["default"] = std::make_unique<Texture2D>(texture);
        }
        else
            DEBUG_WARN("Attempting to overwrite texture 'default'.")
        return GetTexture("default");
    }

    Texture2D *ResourceManager::MakeDummyNormalMap() {
        if(Textures["default_normal"] == nullptr){
            Texture2D texture;

            int width = 1;
            int height = 1;
            unsigned char data[4] = {0, 0, 255, 255};

            texture.internalFormat = GL_RGBA;
            texture.imageFormat = GL_RGBA;

            texture.Generate(width, height, data);
            Textures["default_normal"] = std::make_unique<Texture2D>(texture);
        }
        else
            DEBUG_WARN("Attempting to overwrite texture 'default_normal'.")
        return GetTexture("default_normal");
    }

    Texture2D *ResourceManager::GetTexture(const std::string &name) {
        // always have a default texture at the ready
        if(name == "default" && !Textures["default"].get())
            return MakeDummyTexture();

        if(name == "default_normal" && !Textures["default_normal"].get())
            return MakeDummyNormalMap();

        if(Textures.empty()){
            DEBUG_WARN("No textures created in resource manager system.")
            return nullptr;
        }

        return Textures[name].get();
    }
#pragma endregion

#pragma region Cubemap handling

    Cubemap *ResourceManager::GenerateCubemapFromFiles(const std::vector<std::string> &imagePaths,
                                                       const std::string &cubemapName) {
        // worth considering an 'overwrite' parameter?
        if (Cubemaps[cubemapName] == nullptr /*|| overwrite */){
            Cubemaps[cubemapName] = std::make_unique<Cubemap>(
                    loadCubemapFromFiles(imagePaths));
        }
        else
            DEBUG_WARN("Attempted to overwrite existing texture " + cubemapName + ".")
        return GetCubemap(cubemapName);
    }

    Cubemap *ResourceManager::GetCubemap(const std::string &name) {
        if(Cubemaps.empty()){
            DEBUG_WARN("No cubemaps created in resource manager system.")
            return nullptr;
        }

        return Cubemaps[name].get();
    }

    Cubemap ResourceManager::loadCubemapFromFiles(const std::vector<std::string> &filePaths) {
        Cubemap texture;
        if(filePaths.size() < 6){
            DEBUG_ERROR("Not enough textures provided to generate a valid cubemap! (" + std::to_string(filePaths.size()) + ")")
            return texture;
        }

        unsigned int i = 0;
        for(auto& path: filePaths){
            auto filePath = resolveAssetPath(path);
            stbi_set_flip_vertically_on_load(0);

            std::string pathString = filePath.string();
            const char* filename = pathString.c_str();

            int width, height, nrChannels;
            // try catch here?
            unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

            if(!data){
                DEBUG_ERROR("Failed to load image: " + pathString)
                continue;
            }

            if(nrChannels == 4) {
                texture.internalFormat = GL_RGBA;
                texture.imageFormat = GL_RGBA;
            }

            texture.Generate(i, width, height, data);

            stbi_image_free(data);
            i++;
        }

        texture.SetParams();
        return texture;
    }
#pragma endregion

#pragma region Shader handling
    Shader *ResourceManager::GenerateShaderFromFiles(const fs::path &vertexShaderPath,
                                                     const fs::path &fragmentShaderPath,
                                                     const std::string &shaderName) {
        if(Shaders[shaderName] == nullptr)
            Shaders[shaderName] = std::make_unique<Shader>(
                    loadAndCompileShader(GL_VERTEX_SHADER, vertexShaderPath),
                    loadAndCompileShader(GL_FRAGMENT_SHADER, fragmentShaderPath),
                    shaderName
            );
        return GetShader(shaderName);
    }

    Shader *ResourceManager::GetShader(const std::string &name) {
        if(Shaders.empty()){
            DEBUG_WARN("No shaders created in resource manager system.")
            return nullptr;
        }
        return Shaders[name].get();
    }

    unsigned int ResourceManager::loadAndCompileShader(GLuint shaderType, const fs::path &filePath) {
        auto shaderID = glCreateShader(shaderType);
        auto shaderSource = ReadText(filePath);
        auto source = shaderSource.c_str();
        glShaderSource(shaderID, 1, &source, nullptr);
        glCompileShader(shaderID);

        glStatusData compilationStatus{};
        compilationStatus.shaderName = shaderType == GL_VERTEX_SHADER ? "Vertex" : "Fragment";
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compilationStatus.success);
        if(compilationStatus.success == GL_FALSE) {
            glGetShaderInfoLog(shaderID, GL_INFO_LOG_LENGTH, nullptr, compilationStatus.infoLog);
            DEBUG_RUNTIME_ERROR( std::string(compilationStatus.shaderName) + " shader compilation failed.\n" +
                                 std::string(compilationStatus.infoLog) + "\nPath: " + filePath.string())
        }

        return shaderID;
    }
#pragma endregion

    std::string ResourceManager::ReadText(const fs::path &path) {
        std::ifstream sourceFile(resolveAssetPath(path));
        std::stringstream buffer;
        buffer << sourceFile.rdbuf();
        return buffer.str();
    }

    void ResourceManager::Clear(){
        for (auto it = Textures.begin(); it != Textures.end(); ++it)
            glDeleteTextures(1, &it->second->textureID);
    }
}