#pragma once

#include "engine/ecs/System.h"
#include "engine/utilities/Vector3.h"
#include "engine/utilities/rendering/Material.h"
#include "engine/utilities/Bounds3D.h"

#include <utility>
#include <vector>
#include <unordered_map>
#include <memory>

namespace EisEngine {
    class Game;
    namespace components{
        class PointLight;
        class Mesh3D;
    }
    namespace ecs {
        class Entity;
    }

    namespace systems {
        /// \n A struct extended to a class used to approximate lighting by
        /// aggregating any child light clusters.
        class LightCluster{
            using PointLight = EisEngine::components::PointLight;
            using Mesh3D = EisEngine::components::Mesh3D;
            using Bounds3D = EisEngine::utilities::Bounds3D;
        public:
            /// \n Creates a new Light Cluster.
            explicit LightCluster(
                    PointLight* representative,
                    const float& intensity,
                    std::array<float, 6> bounds,
                    LightCluster* parent = nullptr
                ) :
                representative(representative),
                total_intensity(intensity),
                bounding_box(bounds),
                parent(parent) {}

            explicit LightCluster(
                    PointLight* representative,
                    const float& intensity,
                    Bounds3D& bounds,
                    LightCluster* parent = nullptr
            ) :
                    representative(representative),
                    total_intensity(intensity),
                    bounding_box(bounds),
                    parent(parent) {}

            explicit LightCluster(
                    PointLight* representative,
                    const float& intensity,
                    Bounds3D bounds,
                    LightCluster* parent = nullptr
            ) :
                    representative(representative),
                    total_intensity(intensity),
                    bounding_box(bounds),
                    parent(parent) {}

            [[nodiscard]] bool isLeaf() const { return left == nullptr && right == nullptr;}

            /// \n A tuple of pointers to child clusters. Can be empty.
            /// Children must contain a LightCluster with this->representative == child->representative if not empty.
            std::unique_ptr<LightCluster> left = nullptr;
            std::unique_ptr<LightCluster> right = nullptr;
            LightCluster* parent;

            /// \n A pointer to the cluster's representative light source for material data.
            PointLight* representative = nullptr;

            /// \n A sum of the intensity of all children in the bounding box.
            float total_intensity = 0;

            /// \n x_min, x_max, y_min, y_max, z_min, z_max for the lights in the box
            Bounds3D bounding_box;

            /// \n (Deterministic Barnes-Hut) Determines a cluster's error value.
            [[nodiscard]] float estimateError(const Vector3& pos) const;


        #pragma region parenting stuff
            /// \n Marks the passed light cluster as its child.
            /// Automatically places the child with the highest total intensity left.
            /// If parent cluster already has two children, it will auto-discard the previous right child.
            void AddChild(std::unique_ptr<LightCluster>& cluster){
                // left == nullptr -> implicit check whether any children exist.
                if(left == nullptr){
                    cluster->parent = this;
                    left = std::move(cluster);
                    return;
                }

                // check between clusters which has highest intensity
                bool leftIsStronger = left->total_intensity >= cluster->total_intensity;
                // if existing child is stronger, assign new cluster to right
                if(leftIsStronger) {
                    cluster->parent = this;
                    right = std::move(cluster);
                }
                // otherwise move weaker child to right and assign new cluster to left.
                else{
                    right = std::move(left);
                    cluster->parent = this;
                    left = std::move(cluster);
                }
            }

            std::unique_ptr<LightCluster> DetachChild(bool _left){
                if(_left) {
                    // if pruning left node, reattach any right node to left
                    // (to accommodate for AddChild logic in ordering children)
                    auto temp = std::move(left);
                    if(right != nullptr)
                        left = std::move(right);
                    return temp;
                }
                return std::move(right);
            }

            // called after changing the parenthood to make sure the bounds still fit.
            // recursively tells its parent to update as well.
            void UpdateBBox(){
                // no updates if there aren't both children
                if(left == nullptr || right == nullptr)
                    return;

                bounding_box = Bounds3D::Merge(left->bounding_box, right->bounding_box);
                // exit recursion if no more parent
                if(parent == nullptr)
                    return;

                parent->UpdateBBox();
            }

            void ChooseRepresentative(){
                // exit early if no children or representative is already correct.
                if(isLeaf() || (representative == left->representative))
                    return;

                representative = left->representative;
                // propagate up if changed.
                if(parent != nullptr)
                    parent->ChooseRepresentative();
            }
        #pragma endregion
        };

        class RenderingSystem;

        struct GridCoordHashMap {
            size_t operator()(const Vector3 &c) const {
                // hash numbers from "Optimized spatial hashing for collision detection of
                // deformable objects" - Teschner et.al., 2003.
                // additive instead of XOR for added collision-robustness
                return (std::hash<int>()((int) c.x) * 73856093) +
                       (std::hash<int>()((int) c.y) * 19349663) +
                       (std::hash<int>()((int) c.z) * 83492791);
            }
        };

        class LightSystem : public EisEngine::ecs::System {
            friend class RenderingSystem;
            using Entity = EisEngine::ecs::Entity;
            using LightCluster = EisEngine::systems::LightCluster;
        public:
            /// \n creates an instance of the EisEngine light system.
            explicit LightSystem(Game& engine);

            /// (Deterministic Barnes-Hut) Replaces obsolete QueryNearbyLights
            std::vector<LightCluster*> ComputeLightCut(
                    Vector3& pos,
                    const float& error_threshold
                );

            /// \n (Voxel grid) [OBSOLETE - use ComputeLightCut() instead!] Checks the surroundings of an object for effecting light sources.
            std::vector<int> QueryNearbyLights(const glm::vec3 &objectPos);

            /// \n (Voxel grid) Signals to the light system that an emitting entity has changed position.
            static void MarkLightForUpdate(const int& entityID);

            /// \n (Voxel grid) Registers a light source at the given world position.\n
            /// If said light source is already in the grid, it's old reference is removed.
            void InsertEntityAt(const int& entityID, const Vector3& pos);
            void RemoveEntity(const int& entityID);
            /// \n (Voxel grid) 3D cell size for light source voxel storage; (CELL_SIZE x CELL_SIZE x CELL_SIZE).
            static constexpr float CELL_SIZE = 5.0f;

        private:
            /// \n (Voxel grid) A reference of Point Lights by approximate position in world 3D (x, y, z) space.
            std::unordered_map<Vector3, std::vector<int>, GridCoordHashMap> LightGrid = {};
            /// \n (Voxel grid) A reverse-reference of entity IDs to their bounding voxel.
            std::unordered_map<int, Vector3> entityGridPos = {};

            std::unique_ptr<LightCluster> root = nullptr;
            std::unordered_map<int, LightCluster*> entityTreePos = {};

            /// \n (Voxel grid) Find the voxel a given entity is in.\n
            /// Returns (NaN, NaN, NaN) if the entity is not in the grid, please check against it!
            /// -> std::isnan(result.x) == True if invalid.
            Vector3 FindEntityVoxel(const int& entityID);

            /// \n (Voxel grid) Updates any light sources that have moved since the last frame.
            void UpdateLightSDS();
            void UpdateInGrid(Entity *entity);
            void UpdateInBHTree(Entity* entity);
            void InsertEntityToGrid(const int& entityID, const Vector3& pos);
            void InsertEntityToBHTree(const int& entityID, const Vector3& pos);
            /// \n (Voxel grid) Deregisters a light source from the light grid.
            void RemoveEntityFromGrid(const int& entityID);
            void RemoveClusterFromBHTree(LightCluster* cluster);

            /// \n (Voxel grid) A static reference list of entities whose light sources might need an updated voxel placement.
            static std::vector<unsigned int> lightsToUpdate;
        };
    }
}
