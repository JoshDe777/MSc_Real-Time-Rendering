#include "engine/systems/LightSystem.h"
#include "engine/components/PointLight.h"
#include "engine/Game.h"

#include <limits>
#include <cmath>

namespace EisEngine::systems {
    using PointLight = EisEngine::components::PointLight;

#pragma region non-class utilities:
    inline Vector3 WorldToCell(const glm::vec3& pos) {
        return Vector3{
                floor(pos.x / LightSystem::CELL_SIZE),
                floor(pos.y / LightSystem::CELL_SIZE),
                floor(pos.z / LightSystem::CELL_SIZE)
        };
    }
#pragma endregion

    std::vector<unsigned int> LightSystem::lightsToUpdate = {};

    LightSystem::LightSystem(EisEngine::Game &engine) : System(engine) {
        // on before draw -> update light reference (dynamic 3D SDS)
        engine.onAfterUpdate.addListener([&](Game& engine){
            UpdateLightGrid();
        });
    }

#pragma region light grid handling

    void LightSystem::MarkLightForUpdate(const int &entityID) {
        lightsToUpdate.push_back(entityID);
    }

    void LightSystem::InsertEntityAt(const int &entityID, const Vector3 &pos) {
        // remove entity from grid if exists already
        auto it = entityGridPos.find(entityID);
        if(it != entityGridPos.end())
            RemoveEntityFromGrid(entityID);

        auto cell = WorldToCell(pos);
        LightGrid[cell].push_back(entityID);
        entityGridPos[entityID] = cell;
    }

    void LightSystem::RemoveEntityFromGrid(const int &entityID) {
        auto it = entityGridPos.find(entityID);
        if(it == entityGridPos.end()){
            DEBUG_WARN("Attempting to remove non-existent entity " + std::to_string(entityID) +
                " from light grid.")
                return;
        }

        auto cell = it->second;
        auto& sources = LightGrid[cell];
        sources.erase(std::remove(sources.begin(), sources.end(), entityID),
                      sources.end());

        entityGridPos.erase(it);
    }

    void LightSystem::BuildLightGrid() {
        LightGrid.clear();

        if(engine.componentManager->hasComponentOfType<PointLight>()){
            engine.componentManager->forEachComponent<PointLight>([&](PointLight& light){
                InsertEntityAt(light.GetOwner(), light.position());
            });
        }
    }

    std::vector<int> LightSystem::QueryNearbyLights(const glm::vec3& objectPos) {
        Vector3 c = WorldToCell(objectPos);

        std::vector<int> result;
        result.reserve(16); // fast

        // neighbourhood traversal: get all light sources from the direct surrounding voxels.
        // 3*3*3 = 27 queries.
        for (int dx = -1; dx <= 1; dx++)
            for (int dy = -1; dy <= 1; dy++)
                for (int dz = -1; dz <= 1; dz++) {
                    Vector3 nc{c.x + dx, c.y + dy, c.z + dz};

                    auto it = LightGrid.find(nc);
                    if (it != LightGrid.end()) {
                        const auto &list = it->second;
                        result.insert(result.end(), list.begin(), list.end());
                    }
                }

        return result;
    }

    Vector3 LightSystem::FindEntityVoxel(const int &entityID) {
        auto it = entityGridPos.find(entityID);
        if (it == entityGridPos.end()){
            DEBUG_ERROR("Couldn't resolve entity " + std::to_string(entityID) + " in light grid")
            auto NaN = std::numeric_limits<float>::quiet_NaN();
            return Vector3(NaN, NaN, NaN);
        }

        return it->second;
    }
#pragma endregion

    void LightSystem::UpdateLightGrid() {
        // no update if no light in scene or no light to update.
        if (!engine.componentManager->hasComponentOfType<PointLight>() || lightsToUpdate.empty())
            return;

        // technically means the light will always be a frame forward on transform
        // -> transforms (model matrices) synced before logic update, this is called after logic update.
        // update still works since GetGlobalPosition doesn't reference transform's model matrix
        for(auto owner : lightsToUpdate){
            auto entity = engine.entityManager->getEntity(owner);

            // ignore entity if not attached to a point light.
            if (entity->GetComponent<PointLight>() == nullptr)
                return;

            auto oldPos = FindEntityVoxel(owner);
            auto newPos = entity->transform->GetGlobalPosition();

            // no updates if position didn't change (rotation change also marks as dirty).
            if(oldPos == newPos)
                return;

            RemoveEntityFromGrid(owner);
            InsertEntityAt(owner, newPos);
        }

        // reset updates list.
        lightsToUpdate.clear();
    }
}
