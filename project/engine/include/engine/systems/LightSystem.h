#pragma once

#include "engine/ecs/system.h"
#include "engine/utilities/Vector3.h"
#include "engine/utilities/rendering/Material.h"

#include <vector>
#include <unordered_map>

namespace EisEngine {
    class Game;

    namespace systems {
        class LightCluster{
        public:
            /// \n A tuple of pointers to child clusters. Can be empty.
            /// Children must contain a LightCluster with this->representative == child->representative if not empty.
            std::vector<LightCluster*> children = {};

            /// \n A pointer to the cluster's representative light source for material data.
            PointLight* representative = nullptr;

            /// \n A sum of the intensity of all children in the bounding box.
            float total_intensity = 0;

            /// \n x_min, x_max, y_min, y_max, z_min, z_max for the lights in the box
            std::array<float, 6> bounding_box;

            Vector3 eval(const Vector3& pos);

            /// \n (Deterministic Barnes-Hut) Determines a cluster's error value.
            float getError(const Vector3& pos, Material* mat);
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
        public:
            /// \n creates an instance of the EisEngine light system.
            explicit LightSystem(Game& engine);

            /// (Deterministic Barnes-Hut) Replaces obsolete QueryNearbyLights
            Vector3 ComputeLightCut(Vector3& pos, Material* mat, const float& error_threshold);

            /// \n (Voxel grid) [OBSOLETE - use ComputeLightCut() instead!] Checks the surroundings of an object for effecting light sources.
            std::vector<int> QueryNearbyLights(const glm::vec3 &objectPos);

            /// \n (Voxel grid) Signals to the light system that an emitting entity has changed position.
            static void MarkLightForUpdate(const int& entityID);

            /// \n (Voxel grid) Registers a light source at the given world position.\n
            /// If said light source is already in the grid, it's old reference is removed.
            void InsertEntityAt(const int& entityID, const Vector3& pos);
            /// \n (Voxel grid) Deregisters a light source from the light grid.
            void RemoveEntityFromGrid(const int& entityID);
            /// \n (Voxel grid) 3D cell size for light source voxel storage; (CELL_SIZE x CELL_SIZE x CELL_SIZE).
            static constexpr float CELL_SIZE = 5.0f;

        private:
            /// \n (Voxel grid) A reference of Point Lights by approximate position in world 3D (x, y, z) space.
            std::unordered_map<Vector3, std::vector<int>, GridCoordHashMap> LightGrid = {};
            /// \n (Voxel grid) A reverse-reference of entity IDs to their bounding voxel.
            std::unordered_map<int, Vector3> entityGridPos = {};

            /// \n (Deterministic Barnes-Hut) Builds a Barnes-Hut / Lightcut tree
            void BuildLightHeap();

            /// \n (Voxel grid) Find the voxel a given entity is in.\n
            /// Returns (NaN, NaN, NaN) if the entity is not in the grid, please check against it!
            /// -> std::isnan(result.x) == True if invalid.
            Vector3 FindEntityVoxel(const int& entityID);

            /// \n (Voxel grid) Updates any light sources that have moved since the last frame.
            void UpdateLightGrid();

            /// \n (Voxel grid) A static reference list of entities whose light sources might need an updated voxel placement.
            static std::vector<unsigned int> lightsToUpdate;
        };
    }
}
