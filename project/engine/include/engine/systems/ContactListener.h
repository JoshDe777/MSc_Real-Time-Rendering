#pragma once

#include <box2d/box2d.h>

namespace EisEngine{
    class Game;
    namespace systems{ class PhysicsSystem;}

    namespace physics {
        /// \n The listener looking out for physics collisions.
        class ContactListener : public b2ContactListener {
            friend systems::PhysicsSystem;
        public:
            /// \n Called when two colliders enter collision.
            void BeginContact(b2Contact* contact) override;
            /// \n Called when two colliders exit a collision.
            void EndContact(b2Contact* contact) override;
        private:
            /// \n a pointer to the engine instance.
            Game *engine = nullptr;
        };
    }
}
