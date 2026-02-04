#include "engine/systems/RenderingSystem.h"
#include "engine/Game.h"
#include "engine/Components.h"

#include <algorithm>

// DO NOT UPDATE WITHOUT ALSO UPDATING SAME NAMED MACRO IN FRAGMENT SHADERS!
#define MAX_LIGHTS 1
#define DIST_THRESHOLD 5.0f
#define AMBIENT_FACTOR 0.15f

namespace EisEngine::systems {
// helper functions:
float RenderingSystem::specularFactor = 1.0f;
int RenderingSystem::n_toon_levels = 8;
const std::string RenderingSystem::defaultShader = "Blinn-Phong";
std::string RenderingSystem::active3DShader = "Blinn-Phong";
const std::unordered_map<std::string, std::string> RenderingSystem::shaderNameDict = {
        {"Blinn-Phong", "Blinn-Phong Shader"},
        {"Cook-Torrance", "Cook-Torrance Shader"},
        {"Toon", "Toon Shader"},
        {"Depth", "Depth Mapping"},
        {"Glassy", "Glassy Shader"}
};
Event<RenderingSystem, const Vector2&> RenderingSystem::onResize = Event();

struct Entry{
    PointLight* L;
    float dist2;
};

    // used to sort entities by ascending z position.
    bool CompareZValues(SpriteMesh* a, SpriteMesh* b)
    { return a->entity()->transform->GetGlobalPosition().z < b->entity()->transform->GetGlobalPosition().z;}

// rendering system methods:
    std::vector<Entity*> RenderingSystem::Loaders = {};

    constexpr float CELL_SIZE = 40.0f;

    inline Vector2 WorldToCell(const glm::vec3& pos) {
        return Vector2{
                floor(pos.x / CELL_SIZE),
                floor(pos.z / CELL_SIZE)
        };
    }

    void RenderingSystem::MarkAsLoader(EisEngine::ecs::Entity *ptr) {
        Loaders.push_back(ptr);
    }

    void RenderingSystem::SetSpecularFactor(const float &val) { specularFactor = val;}

    void RenderingSystem::SetActiveShader(const std::string &shaderName) {
        for(const auto& pair : shaderNameDict)
            if(shaderName == pair.first){
                active3DShader = shaderName;
                return;
            }
        DEBUG_WARN("Attempted to set shader to invalid value [" + shaderName + "].")
    }

    void RenderingSystem::InitFBO(const int& index, const Vector2& screenDims) {
        // gen framebuffer
        glGenFramebuffers(1, &FBO[index]);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO[index]);

        // gen depth texture
        glGenTextures(1, &depthTex[index]);
        glBindTexture(GL_TEXTURE_2D, depthTex[index]);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_R32F,
            (int) screenDims.x,
            (int) screenDims.y,
            0,
            GL_RED,
            GL_FLOAT,
            nullptr
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            depthTex[index],
            0
        );

        // gen RBO for depth cache (?)
        glGenRenderbuffers(1, &RBO[index]);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO[index]);
        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH_COMPONENT24,
            (int) screenDims.x,
            (int) screenDims.y
        );

        glFramebufferRenderbuffer(
                GL_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT,
                GL_RENDERBUFFER,
                RBO[index]
        );

        // add draw buffer
        GLenum drawBufs[] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, drawBufs);

        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderingSystem::ResizeFBOItems(const EisEngine::Vector2 &newScreenDims) {
        for(int i = 0; i < FBO.size(); i++){
            glBindRenderbuffer(GL_RENDERBUFFER, FBO[i]);

            glBindTexture(GL_TEXTURE_2D, depthTex[i]);
            glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_R32F,
                    (int) newScreenDims.x,
                    (int) newScreenDims.y,
                    0,
                    GL_RED,
                    GL_FLOAT,
                    nullptr
            );

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // gen RBO for depth cache (?)
            glBindRenderbuffer(GL_RENDERBUFFER, RBO[i]);
            glRenderbufferStorage(
                    GL_RENDERBUFFER,
                    GL_DEPTH_COMPONENT24,
                    (int) newScreenDims.x,
                    (int) newScreenDims.y
            );

            assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    RenderingSystem::RenderingSystem(EisEngine::Game &engine) : System(engine) {
        SetActiveShader("Blinn-Phong");

        camera = &engine.camera;
        if(!camera)
            DEBUG_RUNTIME_ERROR("Cannot initialize rendering; Camera not found.")

        engine.onUpdate.addListener([&] (Game& engine){ Draw();});

        VAO = {};
        for(unsigned int & i : VAO)
            glGenVertexArrays(1, &i);

        int width, height;
        glfwGetWindowSize(engine.getWindow(), &width, &height);
        // init FBOs
        InitFBO(0, Vector2((float) width, (float) height));
        InitFBO(1, Vector2((float) width, (float) height));
        onResize.addListener([this](const Vector2& v){
           ResizeFBOItems(v);
        });
        // add callback to window resize to reallocate depth texture size & rbo sizes on window resize.
        glfwSetWindowSizeCallback(engine.getWindow(), [](GLFWwindow* window, int width, int height){
           RenderingSystem::onResize.invoke(Vector2((float) width, (float) height));
        });

        // generate default shader (mesh2D & lines)
        ResourceManager::GenerateShaderFromFiles("shaders/vert-no_normals.vert",
                                                 "shaders/frag-material_debug_unlit.frag",
                                                 "Default Shader");
        // generate default sprite shader
        ResourceManager::GenerateShaderFromFiles( "shaders/vert-shader3D.vert",
                                                  "shaders/frag-sprite_unlit.frag",
                                                  "Sprite Shader");
        // generate ui sprite shader
        ResourceManager::GenerateShaderFromFiles("shaders/vert-no_normals.vert",
                                                 "shaders/frag-sprite_unlit.frag",
                                                 "UI Shader");
        // generate Blinn-Phong shader
        ResourceManager::GenerateShaderFromFiles("shaders/vert-shader3D.vert",
                                                 "shaders/frag-blinn_phong.frag",
                                                 "Blinn-Phong Shader");
        // generate Cook-Torrance shader
        ResourceManager::GenerateShaderFromFiles("shaders/vert-shader3D.vert",
                                                 "shaders/frag-cook_torrance.frag",
                                                 "Cook-Torrance Shader");
        // generate Toon shader
        ResourceManager::GenerateShaderFromFiles("shaders/vert-shader3D.vert",
                                                 "shaders/frag-toon.frag",
                                                 "Toon Shader");

        // generate Depth Mapping shader
        ResourceManager::GenerateShaderFromFiles("shaders/vert-geometry_debug.vert",
                                                 "shaders/frag-depth_mapping.frag",
                                                 "Depth Mapping");

        // generate Glassy shader
        ResourceManager::GenerateShaderFromFiles("shaders/vert-shader3D.vert",
                                                 "shaders/frag-glassy.frag",
                                                 "Glassy Shader");

        glDisable(GL_CULL_FACE);
    }

    void RenderingSystem::BuildLightGrid() {
        LightGrid.clear();

        if(engine.componentManager.hasComponentOfType<PointLight>()){
            engine.componentManager.forEachComponent<PointLight>([&](PointLight& light){
                auto pos = light.position();
                Vector2 cell = WorldToCell(pos);
                LightGrid[cell].push_back(light.GetOwner());
            });
        }
    }

    std::vector<int> RenderingSystem::QueryNearbyLights(const glm::vec3& objectPos) {
        Vector2 c = WorldToCell(objectPos);

        std::vector<int> result;
        result.reserve(16); // fast

        for (int dx = -1; dx <= 1; dx++) {
            for (int dz = -1; dz <= 1; dz++) {
                Vector2 nc{ c.x + dx, c.y + dz };

                auto it = LightGrid.find(nc);
                if (it != LightGrid.end()) {
                    const auto& list = it->second;
                    result.insert(result.end(), list.begin(), list.end());
                }
            }
        }

        return result;
    }

    void RenderingSystem::PrepareDraw(Mesh3D& mesh, Shader* activeShader){
        auto model = mesh.entity()->transform->GetModelMatrix();
        activeShader->setMatrix("mvp", activeShader->CalculateMVPMatrix(model));
        // model matrices
        activeShader->setMatrix("model", model);
        auto normalMat = glm::mat3(model);
        // if mat is inversible, apply inverse transposed matrix
        normalMat = glm::transpose(glm::inverse(glm::mat3(model)));
        if(abs(glm::determinant(model)) < 1e-6f) {
            // normalize matrix to kill scale variance
            normalMat[0] = glm::normalize(normalMat[0]);
            normalMat[1] = glm::normalize(normalMat[1]);
            normalMat[2] = glm::normalize(normalMat[2]);
        }
        activeShader->setMatrix("normalMat", normalMat);


        // material
        auto renderer = mesh.entity()->GetComponent<Renderer>();
        if(renderer)
            renderer->ApplyData(*activeShader);

        // lighting x LOD
        auto pos = mesh.entity()->transform->GetGlobalPosition();
        //pos.y = 2;
        float lodDist = 100000000000000000.0f;
        if(!Loaders.empty())
            for(auto obj : Loaders){
                auto objPos = obj->transform->GetGlobalPosition();
                //objPos.y = 2;
                lodDist = std::min(lodDist, Vector3::Distance(objPos, pos));
            }

        // if dist to any LOD object < dist threshold
        // compute lighting
        if(lodDist < DIST_THRESHOLD){
            activeShader->setInt("LOD", 1);

            // get lights in grid
            auto results = QueryNearbyLights(pos);
            std::vector<Entry> list;
            list.reserve(results.size());

            // for each entry in the results, create an Entry object
            for (int id : results) {
                auto* e = engine.entityManager.getEntity(id);
                if (!e) continue;

                auto* L = e->GetComponent<PointLight>();
                if (!L) continue;

                float d2 = Vector3::Distance(L->position(), pos);
                list.push_back({L, d2});
            }

            // sort entries by distance
            std::sort(list.begin(), list.end(),
                      [](auto& a, auto& b){ return a.dist2 < b.dist2; });

            // resize list to acceptable size
            if (list.size() > MAX_LIGHTS)
                list.resize(MAX_LIGHTS);

            // unwrap
            for (int i = 0; i < list.size(); i++)
                list[i].L->Apply(*activeShader, i);
            activeShader->setInt("nLights", (int) list.size());
        }
        else{
            // else default to ambient.
            activeShader->setInt("LOD", 0);
        }

        activeShader->setInt("n_levels", n_toon_levels);
    }

    void RenderingSystem::Draw() {
        if(LightGrid.empty())
            BuildLightGrid();

        // re-enable depth testing for 'regular' entities.
        glEnable(GL_DEPTH_TEST);
        auto i = 0;

        #pragma region Default Shader
        auto activeShader = ResourceManager::GetShader("Default Shader");

        // Mesh2D rendering
        if(engine.componentManager.hasComponentOfType<Mesh2D>()){
            glBindVertexArray(VAO[i++]);
            activeShader->Apply(camera);
            engine.componentManager.forEachComponent<Mesh2D>([&](Mesh2D& mesh){
                auto model = mesh.entity()->transform->GetModelMatrix();
                activeShader->setMatrix("mvp", activeShader->CalculateMVPMatrix(model));
                auto renderer = mesh.entity()->GetComponent<Renderer>();
                if(renderer)
                    renderer->ApplyData(*activeShader);
                mesh.draw();
            });
        }

        // line rendering (same shader as Mesh2D's)
        if(engine.componentManager.hasComponentOfType<Line>()){
            glBindVertexArray(VAO[i++]);
            engine.componentManager.forEachComponent<Line>([&] (Line& mesh){
                auto renderer = mesh.entity()->GetComponent<Renderer>();
                if(renderer)
                    renderer->ApplyData(*activeShader);
                auto model = mesh.entity()->transform->GetModelMatrix();
                activeShader->setMatrix("mvp", activeShader->CalculateMVPMatrix(model));
                mesh.draw();
            });
        }
        #pragma endregion

        #pragma region 3D rendering
        // Mesh3D rendering
        activeShader = ResourceManager::GetShader(shaderNameDict.at(active3DShader));
        activeShader->Apply(camera);
        activeShader->setFloat("ambient", AMBIENT_FACTOR);
        activeShader->setFloat("specular", specularFactor);

        std::vector<Mesh3D*> transparentMeshes = {};

        if(engine.componentManager.hasComponentOfType<Mesh3D>()){
            glBindVertexArray(VAO[i++]);
            engine.componentManager.forEachComponent<Mesh3D>([&](Mesh3D& mesh){
                auto renderer = mesh.entity()->GetComponent<Renderer>();
                // early exit if transparent mesh.
                if(renderer && renderer->material->GetOpacity() != 1.0f){
                    transparentMeshes.emplace_back(&mesh);
                    return;
                }

                // no fbos
                PrepareDraw(mesh, activeShader);
                mesh.draw(activeShader->GetShaderID());
            });
        }

        if(!transparentMeshes.empty()){
            activeShader = ResourceManager::GetShader(shaderNameDict.at("Depth"));
            activeShader->Apply(camera);
            activeShader->setFloat("ambient", AMBIENT_FACTOR);
            activeShader->setFloat("specular", specularFactor);

            // bind thickness fbo
            // clear color & depth
            // cull front faces
            glBindFramebuffer(GL_FRAMEBUFFER, FBO[0]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);

            for(auto mesh: transparentMeshes){
                // I think I just need geometry for this one; Edit to fit.
                // PrepareDraw(*mesh, activeShader);
                auto model = mesh->entity()->transform->GetModelMatrix();
                activeShader->setMatrix("mvp", activeShader->CalculateMVPMatrix(model));
                auto view = camera->CalculateViewMatrix();
                activeShader->setMatrix("mv", view * model);
                mesh->draw(activeShader->GetShaderID());
            }

            glBindFramebuffer(GL_FRAMEBUFFER, FBO[1]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_BACK);
            // redo a second pass for front face.

            for(auto mesh: transparentMeshes){
                // I think I just need geometry for this one; Edit to fit.
                // PrepareDraw(*mesh, activeShader);
                auto model = mesh->entity()->transform->GetModelMatrix();
                activeShader->setMatrix("mvp", activeShader->CalculateMVPMatrix(model));
                auto view = camera->CalculateViewMatrix();
                activeShader->setMatrix("mv", view * model);
                mesh->draw(activeShader->GetShaderID());
            }

            // bind "base" fbo (none)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_DEPTH_TEST);
            // turn off depth writing
            glDepthMask(GL_FALSE);
            glDisable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            activeShader = ResourceManager::GetShader(shaderNameDict.at("Glassy"));
            activeShader->Apply(camera);
            activeShader->setFloat("ambient", AMBIENT_FACTOR);
            activeShader->setFloat("specular", specularFactor);
            activeShader->setInt("backDepthMap", 1);
            activeShader->setInt("frontDepthMap", 2);
            auto dims = engine.context.GetWindowSize();
            activeShader->setInt("screenWidth", (int) dims.x);
            activeShader->setInt("screenHeight", (int) dims.y);

            for(auto mesh: transparentMeshes){
                PrepareDraw(*mesh, activeShader);
                // bind back depth texture
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, depthTex[0]);
                // bind front depth texture
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, depthTex[1]);
                mesh->draw(activeShader->GetShaderID());
            }

            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }
        #pragma endregion

        #pragma region Sprite Rendering
        activeShader = ResourceManager::GetShader("Sprite Shader");

        // Sprite rendering

        // weed out UI Sprites for later overlay rendering
        std::vector<SpriteMesh*> uiSprites = {};

        if(engine.componentManager.hasComponentOfType<SpriteMesh>()){
            glBindVertexArray(VAO[i++]);
            activeShader->Apply(camera);
            engine.componentManager.forEachComponent<SpriteMesh>([&] (SpriteMesh& mesh){
                auto renderer = mesh.entity()->GetComponent<Renderer>();
                if(!renderer){
                    DEBUG_ERROR("No sprite renderer attached to mesh on entity " + mesh.entity()->name())
                    return;
                }
                if(renderer->GetLayer() == "UI"){
                    uiSprites.emplace_back(&mesh);
                    return;
                }
                renderer->ApplyData(*activeShader);
                auto model = mesh.entity()->transform->GetModelMatrix();
                activeShader->setMatrix("mvp", activeShader->CalculateMVPMatrix(model));
                mesh.draw();
            });
        }

        // return if no UI sprites to render
        if(uiSprites.empty())
            return;

        // disable depth testing here for UI
        glDisable(GL_DEPTH_TEST);

        // sort by ascending z values for layering because no depth test.
        std::sort(uiSprites.begin(), uiSprites.end(), CompareZValues);

        activeShader = ResourceManager::GetShader("UI Shader");

        glBindVertexArray(VAO[i++]);
        activeShader->Apply(camera);
        for (auto mesh : uiSprites) {
            auto renderer = mesh->entity()->GetComponent<Renderer>();
            renderer->ApplyData(*activeShader);
            auto screenWidth = camera->GetWidth();
            auto screenHeight = camera->GetHeight();
            auto projection = glm::ortho(-(float) screenWidth / 2, (float) screenWidth / 2,
                                         - (float) screenHeight / 2, (float) screenHeight / 2);
            auto modelProjection = projection * mesh->entity()->transform->GetModelMatrix();
            activeShader->setMatrix("mvp", modelProjection);
            mesh->draw();
        }
        #pragma endregion
    }
}
