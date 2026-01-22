#include "engine/systems/RenderingSystem.h"
#include "engine/Game.h"
#include "engine/Components.h"

#include <algorithm>

// DO NOT UPDATE WITHOUT ALSO UPDATING SAME NAMED MACRO IN FRAGMENT SHADERS!
#define MAX_LIGHTS 3
#define DIST_THRESHOLD 5.0f
#define AMBIENT_FACTOR 0.15f

namespace EisEngine::systems {
// helper functions:
float RenderingSystem::specularFactor = 1.0f;

struct Entry{
    PointLight* L;
    float dist2;
};

    // used to sort entities by ascending z position.
    bool CompareZValues(SpriteMesh* a, SpriteMesh* b)
    { return a->entity()->transform->GetGlobalPosition().z < b->entity()->transform->GetGlobalPosition().z;}

// rendering system methods:
    std::vector<Entity*> RenderingSystem::Loaders = {};

    constexpr float CELL_SIZE = 4.0f;

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

    RenderingSystem::RenderingSystem(EisEngine::Game &engine) : System(engine) {
        camera = &engine.camera;
        if(!camera)
            DEBUG_RUNTIME_ERROR("Cannot initialize rendering; Camera not found.")

        engine.onUpdate.addListener([&] (Game& engine){ Draw();});

        VAO = {};
        for(unsigned int & i : VAO)
            glGenVertexArrays(1, &i);

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
        // generate 3D shader
        ResourceManager::GenerateShaderFromFiles("shaders/vert-shader3D.vert",
                                                 "shaders/frag-blinn_phong.frag",
                                                 "Blinn-Phong Shader");

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
        activeShader = ResourceManager::GetShader("Blinn-Phong Shader");
        activeShader->setFloat("ambient", AMBIENT_FACTOR);
        activeShader->setFloat("specular", specularFactor);
        if(engine.componentManager.hasComponentOfType<Mesh3D>()){
            glBindVertexArray(VAO[i++]);
            activeShader->Apply(camera);
            engine.componentManager.forEachComponent<Mesh3D>([&](Mesh3D& mesh){
                auto model = mesh.entity()->transform->GetModelMatrix();
                activeShader->setMatrix("model", model);
                activeShader->setMatrix("mvp", activeShader->CalculateMVPMatrix(model));
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
                auto renderer = mesh.entity()->GetComponent<Renderer>();
                if(renderer)
                    renderer->ApplyData(*activeShader);

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

                mesh.draw(activeShader->GetShaderID());
            });
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
