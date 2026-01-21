#pragma once

#include <box2d/box2d.h>
#include "engine/ecs/Component.h"
#include "engine/components/Transform.h"
#include "engine/Utilities.h"

namespace EisEngine {
    namespace systems{ class PhysicsUpdater;}
    class Game;

    using systems::PhysicsUpdater;
    using namespace EisEngine::ecs;

    namespace components {
        /// \n This component represents an entity's physical presence in 2D world space.
        /// it bases on the Box2D physics simulation and is an encapsulation of the b2Body type.
        /// \n REQUIRES a PREVIOUSLY CREATED BoxCollider2D component for proper use!
        class PhysicsBody2D : public Component {
            friend class BoxCollider2D;
            friend ComponentManager;
            friend PhysicsUpdater;
            friend Entity;
        public:
            /// \n Creates a Body component using Box2D.
            /// @param type - a b2BodyType defining the type of body, defaulting as a dynamic body.
            /// For a full list of options please refer to the box2d API.
            PhysicsBody2D(Game &engine, guid_t owner, b2BodyType type = b2_dynamicBody);

            /// \n Applies the given force once to the rigidbody.
            /// @param force - Vector2: the direction and strength of the force to be applied.
            void AddForce(const Vector2 &force) const;

            /// \n Returns the body's current velocity.
            [[nodiscard]] Vector2 GetVelocity() const;
            /// \n Returns the body's current friction value.
            [[nodiscard]] float GetFriction() const;
            /// \n Returns the body's current density value.
            [[nodiscard]] float GetDensity() const;
            /// \n Returns the body's current restitution or bounciness value.
            [[nodiscard]] float GetBounciness() const;
            /// \n Returns whether body is a sensor or not.
            /// \n If true, the body does not react physically to collisions.
            [[nodiscard]] bool IsSensor() const;
            /// \n Returns whether or not the body is affected by physics in general or not.
            /// \n If true, the body is affected by physics.
            [[nodiscard]] bool IsKinematic() const;
            /// \n Returns whether the body is affected by gravity or not.
            [[nodiscard]] bool UsesGravity() const;

            /// \n Sets the body's linear velocity to a given value.
            void SetVelocity(const Vector2& velocity) const;
            /// \n Sets the body's friction to a given value.
            void SetFriction(float value);
            /// \n Sets the body's density to a given value.
            void SetDensity(float value);
            /// \n Sets the body's bounciness to a given value.
            void SetBounciness(float value);
            /// \n Determines whether the current rigidbody counts as a sensor.
            /// @param value - if true, body will detect collisions but not physically react to them\n
            /// if false reacts to collisions physically as well.
            void SetSensorStatus(bool value);
            /// \n Sets whether or not the body is affected by gravity.
            /// @param value - if true, body will be affected by world gravity\n
            /// if false, body will not be affected by gravity whatsoever.
            void SetGravityUse(bool value);

            /// \n Instructs the physics body to ignore all collisions.
            void IgnoreAllCollisions() const;
            /// \n Instructs the physics body to stop ignoring collisions.
            void EnableAllCollisions() const;

            /// \n The b2Body linking EisEngine to Box2D's physics engine.
            b2Body* body = nullptr;
        private:
            /// \n Function called when a component is intentionally deleted.
            void Invalidate() override;

            /// \n Synchronizes the body's values with the physics engine's updates.
            void SyncPhysics();

            /// \n The transform associated with the rigidbody.
            Transform *transform = nullptr;

            /// \n Determines whether the body is a sensor (objects can pass through but collisions are detected).
            bool isSensor = false;
            /// \n (Useless) Determines whether the body is kinematic or not (If not, physics simulation will discard it).
            bool isKinematic = true;
            /// \n Determines whether the body is affected by gravity.
            bool usesGravity = true;

            /// \n The body's density value.
            /// \n Higher density will result in greater mass per surface unit, making the body 'heavier'
            float density = 1;
            /// \n The body's friction value.
            /// \n The higher the friction value, the faster the body's will decelerate.
            float friction = 1;
            /// \n The body's restitution or bounciness value.
            /// \n The higher the bounciness, the further an object will 'bounce' off another collider.
            float bounciness = 0;

            /// \n The x and y scales of the rigidbody.
            Vector2 scale = Vector2::one;
            /// \n The body's primitive shape.
            std::vector<b2Vec2> rawVertices;
        };
    }
}
