#pragma once

#include <set>
#include <glm/glm.hpp>
#include "engine/ecs/Component.h"
#include "engine/Utilities.h"

using EisEngine::ecs::Component;
using EisEngine::ecs::ComponentManager;
using EisEngine::ecs::Entity;

namespace EisEngine{
    namespace systems{ class SceneGraphUpdater;}

    namespace components {
        class PhysicsBody2D;

        /// \n Represents an entity's transform data.
        /// \n It is heavily advised NOT to add more than one transform to an entity,
        /// which automatically creates one on creation.
        class Transform final: public Component {
            friend ComponentManager;
            friend Entity;
            friend PhysicsBody2D;
            friend systems::SceneGraphUpdater;
        public:
            /// \n Creates a new transform.
            explicit Transform(Game &engine,
                               guid_t owner,
                               Transform *parentTransform = nullptr,
                               Vector3 position = Vector3::zero,
                               Vector3 rotation = Vector3::zero,
                               Vector3 scale = Vector3::one);

            Transform(const Transform&) = delete;
            Transform& operator = (const Transform&) = delete;
            Transform& operator = (Transform&&) = delete;
            Transform(Transform &&other) noexcept;

            /// \n Function called when the component is marked for deletion as a component
            void Invalidate() override;

            /// \n Returns the object's position relative to the world origin.
            [[nodiscard]]Vector3 GetGlobalPosition();
            /// \n Returns the object's rotation relative to the world origin.
            [[nodiscard]]Vector3 GetGlobalRotation() const;
            /// \n  Returns the object's scale relative to the world origin.
            [[nodiscard]] Vector3 GetGlobalScale();
            /// \n Returns the object's position relative to its parent object.
            [[nodiscard]]Vector3 GetLocalPosition(){ return localPosition;}
            /// \n Returns the object's rotation relative to its parent object.
            [[nodiscard]]Vector3 GetLocalRotation(){ return localRotation;}
            /// \n Returns the object's scale relative to its parent object.
            [[nodiscard]] Vector3 GetLocalScale(){ return localScale;}
            [[nodiscard]] bool IsDirty() const { return dirty;}

            /// \n Returns the transform's model matrix, condensing full transform data in one object.
            [[nodiscard]] glm::mat4 GetModelMatrix(){ return modelMatrix;}
            /// \n Returns a transform's local model matrix, condensing local transform data into one object.
            glm::mat4 GetLocalMatrix();

            /// \n Sets transform position in world space.
            void SetGlobalPosition(const Vector3& pos);
            /// \n Sets transform rotation in world space.
            void SetGlobalRotation(const Vector3& rotation);
            /// \n Sets transform scale in world space.
            void SetGlobalScale(const Vector3& scale);
            /// \n Sets transform position relative to its parent object.
            void SetLocalPosition(const Vector3& pos);
            /// \n Sets transform rotation relative to its parent object.
            void SetLocalRotation(const Vector3& rotation);
            /// \n Sets transform scale relative to its parent object.
            void SetLocalScale(const Vector3& scale);

            /// \n Gets an entity's child entities in the scene graph.
            /// @return std::set&lt;Transform *>: \n a set of pointers to the child entities' Transform components.
            std::set<Transform *> getChildren() { return children;}
            /// \n Gets the transform's parent transform.
            /// @return Transform*: a pointer to the parent transform component.
            Transform *parent() { return m_parent;}
            /// \n Sets a new parent transform.
            /// @param transform - Transform*: a pointer to the parent-to-be transform.
            void SetParent(Transform *transform);

            /// \n Moves the transform by the given vector.
            void Translate(const Vector3 &direction);
            /// \n Rotates the transform by the given Euler angles.
            void Rotate(const Vector3 &vector);
            /// \n Scales the transform with the given vector.
            void Rescale(const Vector3& scalingFactors);

            // All transform::Direction() functions assume objects are created facing negative Z.
            /// \n Calculates the direction to the right hand side of an object, assuming it started facing negative Z.
            [[nodiscard]] Vector3 Right() const { return Vector3::right.Rotate(localRotation).normalized();}
            /// \n Calculates the upwards direction of an object, assuming it started facing negative Z.
            [[nodiscard]] Vector3 Up() const { return Vector3::up.Rotate(localRotation).normalized();}
            /// \n Calculates the forwards direction of an object, assuming it started facing negative Z.
            [[nodiscard]] Vector3 Forward() const { return Vector3::forward.Rotate(localRotation).normalized();}
        private:
            /// \n Adds a child Transform.
            void AddChild(Transform *transform);
            /// \n Removes a given child from the children list.
            void RemoveChild(Transform *transform);

            /// \n Represents transform position relative to its parent entity.
            Vector3 localPosition;
            /// \n Represents transform rotation relative to its parent entity.
            Vector3 localRotation;
            /// \n Represents transform scale relative to its parent entity.
            Vector3 localScale;
            /// \n Represents the transform data in global space as a 4x4 matrix.
            glm::mat4 modelMatrix;

            /// \n Indicates whether the transform's position was changed manually in the current frame.
            bool m_positionChanged = false;
            /// \n Indicates whether the transform's rotation was changed manually in the current frame.
            bool m_rotationChanged = false;
            /// \n Indicates whether the transform's position was changed manually in the current frame.
            bool m_scaleChanged = false;
            /// \n pointer to the transform's parent transform.
            Transform *m_parent = nullptr;
            /// \n a set of transform pointers assigned as the current transform's children.
            std::set<Transform *> children;
            /// \n flags whether the object was changed directly in the world.
            bool dirty = true;

            /// \n Syncs global position to the physics body's.
            void SyncPosition(const Vector3& newPosition);
            /// \n Syncs global rotation to the physics body's.
            void SyncRotation(const Vector3& newRotation);
            /// \n Syncs global scale to the collider's.
            void SyncScale(const Vector3& oldScale, const Vector3& newScale);

            void MarkDirty();
        };
    }
}

