#include "engine/systems/LightSystem.h"
#include "engine/Game.h"
#include "engine/components/PointLight.h"
#include "engine/components/meshes/Mesh3D.h"
#include "engine/utilities/Bounds3D.h"

#include <limits>
#include <cmath>
#include <queue>

namespace EisEngine::systems {
    using PointLight = EisEngine::components::PointLight;
    using Mesh3D = EisEngine::components::Mesh3D;
    using Bounds3D = EisEngine::utilities::Bounds3D;

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
            UpdateLightSDS();
            // root = BuildLightHeap();
        });
    }

#pragma region light grid handling

    void LightSystem::MarkLightForUpdate(const int &entityID) {
        lightsToUpdate.push_back(entityID);
    }

    void LightSystem::InsertEntityAt(const int &entityID, const Vector3 &pos) {
        InsertEntityToGrid(entityID, pos);
        InsertEntityToBHTree(entityID, pos);
    }

    void LightSystem::RemoveEntity(const int &entityID) {
        RemoveEntityFromGrid(entityID);
        auto cluster = entityTreePos.at(entityID);
        if (cluster != nullptr) {
            RemoveClusterFromBHTree(cluster);
            entityTreePos.erase(entityID);
        }
    }

    void LightSystem::InsertEntityToGrid(const int &entityID, const EisEngine::Vector3 &pos) {
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

#pragma region dynamic SDS stuff

    void LightSystem::UpdateLightSDS() {
        // no updates to make if no lights in scene or to update
        if(!engine.componentManager->hasComponentOfType<PointLight>() || lightsToUpdate.empty())
            return;

        for(auto id : lightsToUpdate){
            auto entity = engine.entityManager->getEntity((int) id);
            UpdateInGrid(entity);
            UpdateInBHTree(entity);
        }

        lightsToUpdate.clear();
    }

    void LightSystem::UpdateInGrid(Entity *entity) {

        // technically means the light will always be a frame forward on transform
        // -> transforms (model matrices) synced before logic update, this is called after logic update.
        // update still works since GetGlobalPosition doesn't reference transform's model matrix
        // ignore entity if not attached to a point light.
        if (entity->GetComponent<PointLight>() == nullptr)
            return;

        auto owner = entity->guid();

        auto oldPos = FindEntityVoxel(owner);
        auto newPos = entity->transform->GetGlobalPosition();

        // no updates if position didn't change (rotation change also marks as dirty).
        if(oldPos == newPos)
            return;

        RemoveEntityFromGrid(owner);
        InsertEntityToGrid(owner, newPos);
    }

    void LightSystem::UpdateInBHTree(EisEngine::systems::LightSystem::Entity *entity) {
        auto light = entity->GetComponent<PointLight>();
        if(light == nullptr)
            return;

        // find light in BHTree
        auto cluster = entityTreePos.at(entity->guid());
        // if light is already in the tree
        if(cluster != nullptr){
            // if position hasn't changed, return
            bool pos_changed = light->position() != cluster->bounding_box.Centre();
            if(!pos_changed)
                return;

            // otherwise prune it from the tree.
            RemoveClusterFromBHTree(cluster);
            entityTreePos.erase(entity->guid());
        }

        // (re-)insert light source somewhere else.
        InsertEntityToBHTree(entity->guid(), light->position());
    }

#pragma endregion

#pragma region barnes-hut stuff
    float LightCluster::estimateError(const Vector3 &pos) const {
        // get closest point from bounding box to pos
        Vector3 closest = bounding_box.GetClosestPointTo(pos);
        auto d_min = max(Vector3::Distance(closest, pos), Math::EPSILON);

        // square attenuation 1 / d^2:
        auto geom = 1.0f / (d_min*d_min);

        // maximum value of specular lobe
        auto brdf = RenderingSystem::GetMaxBRDF();

        return total_intensity * geom * brdf;
    }

    // custom comparison metric for clusters to ensure priority queues select the cluster with the min cost.
    auto compareClustersBuild = [](std::pair<LightCluster*, float>& a, std::pair<LightCluster*, float>& b){
        return a.second > b.second;
    };
    // as above but opposite order (building choosing roughest clusters to refine).
    auto compareClustersRun = [](std::pair<LightCluster*, float>& a, std::pair<LightCluster*, float>& b){
        return a.second < b.second;
    };

    std::vector<LightCluster*> LightSystem::ComputeLightCut(
            EisEngine::Vector3 &pos,
            const float &error_threshold
    ) {
        // return an empty list if there are no lights registered.
        if(root == nullptr)
            return {};

        std::vector<LightCluster*> results = {};

        // custom max-queue with function assessing max error as largest priority.
        auto q = std::priority_queue<
                std::pair<LightCluster*, float>,
                std::vector<std::pair<LightCluster*, float>>,
                decltype(compareClustersRun)>(compareClustersRun);
        q.push({root.get(), root->estimateError(pos)});

        // until there is no more cluster to be subdivided.
        while(!q.empty()){

            auto r = q.top();
            q.pop();

            auto node = r.first;
            auto error = r.second;

            // if the cluster's computed error factor is good enough, or we're evaluating the light source itself,
            // store the node and continue to the next branch.
            if (error < error_threshold || node->isLeaf()){
                results.push_back(node);
                continue;
            }

            // otherwise calculate children's errors and add them to the queue.
            auto lErr = node->left->estimateError(pos);
            q.push({node->left.get(), lErr});

            auto rErr = node->right->estimateError(pos);
            q.push({node->right.get(), rErr});
        }

        // return list of valid clusters for the light cut.
        return results;
    }

    void LightSystem::InsertEntityToBHTree(const int &entityID, const EisEngine::Vector3 &pos) {
        auto entity = engine.entityManager->getEntity(entityID);
        auto light = entity->GetComponent<PointLight>();

        auto newCluster = std::make_unique<LightCluster>(
            light,
            light->GetIntensity(),
            Bounds3D(light->position())
        );
        // valid assuming nodes don't get deleted.
        entityTreePos.insert({entityID, newCluster.get()});

        // if no light tree in scene, insert node as only lightcluster
        if (root == nullptr){
            root = std::move(newCluster);
            return;
        }

        LightCluster* bestSibling = root.get();
        float bestCost = 100000000000000.0;

        // "custom" min-prio-Q based on lambda function compareClustersBuild (defined above ComputeLightCut)
        auto q = std::priority_queue<
                std::pair<LightCluster*, float>,
                std::vector<std::pair<LightCluster*, float>>,
                decltype(compareClustersBuild)> (compareClustersBuild);
        std::pair<LightCluster*, float> pair = {root.get(), 0};
        q.push(pair);

        while(!q.empty()){
            auto r = q.top();
            q.pop();
            auto node = r.first;
            auto costToNode = r.second;

            // cost of node = difference in bounding box size
            auto size = node->bounding_box.diagonalSize();
            auto potSize = node->bounding_box.Expand(pos).diagonalSize();
            auto directCost = potSize - size;

            auto totalCost = costToNode + directCost;
            if (totalCost < bestCost){
                bestCost = totalCost;
                bestSibling = node;
            }

            // pruning of subtrees where inherited cost >= bestCost
            // (bounds won't get smaller -> not worth looking into)
            if (costToNode >= bestCost || node->isLeaf())
                continue;

            q.push({node->left.get(), totalCost});
            q.push({node->right.get(), totalCost});
        }

        auto rep = bestSibling->total_intensity > light->GetIntensity() ?
                   bestSibling->representative :
                   light;

        // edge case: best sibling is root:
        if (bestSibling == root.get()){
            // implant new depth=1 subtree of siblings.
            auto newRoot = std::make_unique<LightCluster>(
                rep,
                bestSibling->total_intensity + light->GetIntensity(),
                Bounds3D::Merge(bestSibling->bounding_box, newCluster->bounding_box)
            );
            newRoot->AddChild(root);
            newRoot->AddChild(newCluster);
            root = std::move(newRoot);
            return;
        }

        auto oldParent = bestSibling->parent;
        // checks whether the best sibling is assigned to left or not
        bool isLeft = oldParent->left.get() == bestSibling;

        std::unique_ptr<LightCluster> sibling_ptr = oldParent->DetachChild(isLeft);

        auto newParentCluster = std::make_unique<LightCluster>(
            // determine representative light etc here
            rep,
            sibling_ptr->total_intensity + light->GetIntensity(),
            Bounds3D::Merge(sibling_ptr->bounding_box, newCluster->bounding_box)
        );

        // attach sibling & new node to subtree (order irrelevant since reordered in 2nd AddChild)
        newParentCluster->AddChild(sibling_ptr);
        newParentCluster->AddChild(newCluster);

        // reattach subtree to old branch last
        // -> AddChild does std::move on the unique_ptr passed
        // update bounding boxes for all clusters above selected.
        oldParent->AddChild(newParentCluster);
        oldParent->UpdateBBox();
    }

    void LightSystem::RemoveClusterFromBHTree(EisEngine::systems::LightCluster *cluster) {
        auto affectedLight = cluster->representative;

        // edge case: cluster is root:
        if(cluster == root.get()){
            // no more lights in scene
            root = nullptr;
            return;
        }

        auto parent = cluster->parent;
        bool siblingIsLeft = parent->left.get() != cluster;

        // edge case: cluster is child of root
        if(parent == root.get()){
            // make sibling root
            auto sibling = parent->DetachChild(siblingIsLeft);
            root = std::move(sibling);
            return;
        }

        // get family tree
        auto grandparent = parent->parent;
        // establish left or right sub-branch by comparing pointer addresses.
        bool parentIsLeft = grandparent->left.get() == parent;

        // unlink parent from grandparent & sibling from parent
        auto orphanParent = grandparent->DetachChild(parentIsLeft);
        auto orphanSibling = parent->DetachChild(siblingIsLeft);

        // attach sibling to grandparent (bounding boxes can only get smaller so works)
        grandparent->AddChild(orphanSibling);
        grandparent->UpdateBBox();

        // link sibling to grandparent -> fine because bounding box can only get smaller
        // if granparent.representative isn't cluster.representative can return safely
        if (grandparent->representative != affectedLight)
            return;

        // otherwise rechoose representative & propagate up (ChooseRepresentative() already handles recursion)
        grandparent->ChooseRepresentative();
    }
#pragma endregion
}
