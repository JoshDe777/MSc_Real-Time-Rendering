#pragma once

#include <box2d/box2d.h>
#include "ContactListener.h"
#include "engine/ecs/System.h"
#include "engine/Utilities.h"

namespace EisEngine {
    using namespace ecs;
    using physics::ContactListener;
    namespace components{ class PhysicsBody2D;}
    using components::PhysicsBody2D;

    namespace systems {
        /// \n This system runs the game world's physics simulation.
        class PhysicsSystem : public System {
        public:
            /// \n The standard earth gravity vector (0, -9.81).
            static const Vector2 EarthGravityVector;

            /// \n Creates a new physics engine for the game, with default gravity being Earth's 9.81 towards negative y
            /// @param game - a reference to the engine systems to add physics simulations into the game loop.
            /// @param _gravity - a 2D vector describing the desired gravity in the world. Defaults to (0, -9.81).
            explicit PhysicsSystem(Game &game, Vector2 _gravity = EarthGravityVector);

            /// \n Disables gravity for the current physics simulation.
            void DisableGlobalGravity(){ SetGlobalGravity(Vector2::zero);}
            /// \n Enables gravity for the current physics simulation at its last value.
            void EnableGlobalGravity(){ SetGlobalGravity(gravity);}
            /// \n Sets the global gravity to the given value.
            void SetGlobalGravity(Vector2 newGravity);

            /// \n Creates a physical body based on a box2d definition.
            /// @param bodyDef - a b2Body definition serving as a blueprint for objects of this type.
            b2Body* CreateBody(b2BodyDef &bodyDef);
            /// \n Marks a body for deletion and deletes it at the next possible moment.
            /// @param body - b2Body*: A pointer to the b2 body to be deleted.
            static void DeleteBody(b2Body* body);

            /// \n Returns a list of all bodies within the given bounds.
            /// \n Vectors should be arranged to form a square with edges &lt;v1-v2, v2-v3, v3-v4, v4-v1>.
            static std::vector<PhysicsBody2D*> FindBodiesWithinBounds(const Vector2& v1,
                                                                      const Vector2& v2,
                                                                      const Vector2& v3,
                                                                      const Vector2& v4);
        private:
            /// \n Simulates physics for the current frame before deleting bodies marked for deletion.
            void Step();

            /// \n The current gravity vector. \n Defaults to Earth gravity (0, -9.81).
            Vector2 gravity =  EarthGravityVector;
            /// \n The b2 physics engine.
            b2World physicsWorld{gravity};
            /// \n The collision listener.
            ContactListener contactListenerInstance;

            /// \n Each iteration searches for collisions, leading to more accurate collision detection.
            const int32_t velocityIterations = 10;
            /// \n Each iteration adjusts object positions post-collision, leading to more accurate collision behaviour.
            const int32_t positionIterations = 8;

            /// \n A list of b2Bodies scheduled for removal.
            static std::vector<b2Body*> bodiesToDelete;
            /// \n A pointer to the engine instance.
            static Game* engine;
        };
    }
}
