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
                continue;

            auto oldPos = FindEntityVoxel(owner);
            auto newPos = entity->transform->GetGlobalPosition();

            // no updates if position didn't change (rotation change also marks as dirty).
            if(oldPos == newPos)
                continue;

            RemoveEntityFromGrid(owner);
            InsertEntityAt(owner, newPos);
        }

        // reset updates list.
        lightsToUpdate.clear();
    }

#pragma region barnes-hut stuff
    Vector3 LightCluster::eval(const EisEngine::Vector3 &pos) {
        // L
        Vector3 posToLight = representative->position() - pos;
        // dist
        float dist = posToLight.magnitude();
        posToLight = posToLight.normalized();
        // N
        // V(iew) -> requires Mesh.GetNormalAt(pos)

        // cos_in = max(N.dot(L), 0.0f);

        // attenuation = cos_in / max(dist*dist, epsilon);

        // brdf = mat->eval(N, posToLight, V);

        // return total_intensity * attenuation * brdf;
    }

    float LightCluster::getError(const EisEngine::Vector3 &pos, Material *mat) {
        // get closest point from bounding box to pos
        // d_min = max(dist(closest - pos), epsilon)

        // square attenuation:
        // geom = 1.0 / (d_min*d_min)

        // cos = 1 (could bound with normal but cba)

        // maximum value of specular lobe
        // brdf = mat->get_max_brdf("Blinn-Phong")

        // intensity = total_intensity

        // return intensity * geom * cos * brdf;
    }

    Vector3 LightSystem::ComputeLightCut(
            EisEngine::Vector3 &pos,
            EisEngine::Material *mat,
            const float &error_threshold
    ) {
        Vector3 radiance = Vector3();

        // q = PriorityQueue();
        // q.push(root, priority=root.getError(pos, mat))

        // while (!q.empty()) {
            // node, bound = q.pop_max();
            // if (bound < error_threshold || node->children.size() == 0){
                // radiance += node->eval(pos)
                // continue;
            // }
            // for(auto child: node->children){
                // bound = child->getError(pos, mat);
                // q.push(child, mat);
            // }
        // }

        return radiance;
    }

    void LightSystem::BuildLightHeap() {
        // build light tree here

        // greedy bottom-up:
        // next pair = smallest new cluster with metric = diagonal bounding box length;
        // LightCluster.representative = highest intensity child.
    }
#pragma endregion
}
