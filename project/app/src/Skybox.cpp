#include "Skybox.h"

namespace RTR {
    Skybox::Skybox(Game& engine) {
        entity = static_cast<shared_ptr<Entity>>(&engine.entityManager.createEntity("Skybox"));
        entity->AddComponent<Mesh3D>(PrimitiveMesh3D::skybox);
        auto cubemap = ResourceManager::GenerateCubemapFromFiles(skyboxPaths, "skybox");
        entity->AddComponent<CubemapRenderer>(cubemap);
        RenderingSystem::SetSkyboxEntity(entity.get());
    }
}