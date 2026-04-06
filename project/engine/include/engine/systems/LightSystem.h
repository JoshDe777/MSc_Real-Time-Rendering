#pragma once

#include "engine/ecs/system.h"
#include "engine/utilities/Vector3.h"

#include <vector>
#include <unordered_map>

namespace EisEngine {
    class Game;

    namespace systems {
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

            /// \n Checks the surroundings of an object for effecting light sources.
            std::vector<int> QueryNearbyLights(const glm::vec3 &objectPos);

            /// \n Signals to the light system that an emitting entity has changed position.
            static void MarkLightForUpdate(const int& entityID);

            /// \n Registers a light source at the given world position.\n
            /// If said light source is already in the grid, it's old reference is removed.
            void InsertEntityAt(const int& entityID, const Vector3& pos);
            /// \n Deregisters a light source from the light grid.
            void RemoveEntityFromGrid(const int& entityID);

            static constexpr float CELL_SIZE = 5.0f;

        private:
            /// \n A reference of Point Lights by approximate position in world 3D (x, y, z) space.
            std::unordered_map<Vector3, std::vector<int>, GridCoordHashMap> LightGrid = {};
            std::unordered_map<int, Vector3> entityGridPos = {};

            /// Find the voxel a given entity is in.\n
            /// Returns (NaN, NaN, NaN) if the entity is not in the grid, please check against it!
            /// -> std::isnan(result.x) == True if invalid.
            Vector3 FindEntityVoxel(const int& entityID);

            /// \n Updates any light sources that have moved since the last frame.
            void UpdateLightGrid();

            static std::vector<unsigned int> lightsToUpdate;
        };
    }
}
