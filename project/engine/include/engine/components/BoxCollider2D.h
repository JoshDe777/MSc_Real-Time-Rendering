#pragma once

#include "engine/ecs/Component.h"
#include "engine/Events.h"

namespace EisEngine{
    namespace physics{
        class ContactListener;
    }
    using namespace events;
    using namespace ecs;
    using namespace physics;

    namespace components {
        /// \n This component represents an entity's ability to interact with other entities through collisions and triggers.
        /// \n Requires a PhysicsBody component for proper use.
        class BoxCollider2D: public Component {
            friend ContactListener;
            friend class PhysicsBody2D;
            friend class Transform;
            using event_t = events::Event<BoxCollider2D, BoxCollider2D&>;
        public:
            /// \n Creates a BoxCollider2D Component. Bounds will be set to fully contain any mesh,
            /// or in its absence a simple 1x1 box;
            explicit BoxCollider2D(Game &engine, guid_t owner);

            /// \n An event called whenever this collider enters contact with another collider.
            event_t OnCollisionEnter;
            /// \n An event called whenever this collider exits contact with another collider.
            event_t OnCollisionExit;

            /// \n Gets the collider's boundaries.
            [[nodiscard]] Bounds2D* GetBounds() { return &bounds;}
            /// \n Sets new boundaries for the collider.
            void SetBounds(Bounds2D newBounds);
        protected:
            /// \n A function called when a component is intentionally deleted.
            void Invalidate() override;
        private:
            /// \n Function called when the collider enters collision with another collider.
            void EnterCollision(BoxCollider2D &other);
            /// \n Function called when the collider ends a collision with another collider.
            void ExitCollision(BoxCollider2D &other);

            /// \n Automatically searches for meshes and creates a box around it.
            /// \n If there is no mesh, creates a 1x1 cube.
            Bounds2D SetDefaultBounds(const Bounds2D& _defaultBounds = Bounds2D::Square1x1);
            /// \n Array of 2D boundaries for the collider.
            /// \n Organized in the following order: top, right, bottom, left (clockwise).
            Bounds2D bounds;
            /// \n Edits the bounds to fit the new scale, using the old scale as reference to normalize.
            void RescaleBounds(const Vector3& oldScale, const Vector3& scale);
        };
    }
}
