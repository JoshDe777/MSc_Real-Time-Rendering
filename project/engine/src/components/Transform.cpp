#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "engine/components/Transform.h"
#include "engine/Game.h"
#include "engine/components/BoxCollider2D.h"

namespace EisEngine::components{
// helper functions:

    // normalize an angle to the range [0, 360].
    float NormalizeAngle(const float& angle){
        float modAngle = fmod(angle, 360.0f);               // clamp to 360 degrees
        return modAngle < 0 ? modAngle + 360.0f : modAngle;         // ensure positive value
    }

    // normalize all angles in a vector to [0, 360].
    Vector3 NormalizeAngles(const Vector3& v){
        return Vector3( NormalizeAngle(v.x),NormalizeAngle(v.y), NormalizeAngle(v.z));
    }

    // extract euler rotation data from a matrix.
    inline Vector3 ConvertMatrixToEuler(const glm::mat4& mat) {
        glm::vec3 euler;
        glm::extractEulerAngleYXZ(mat, euler.y, euler.x, euler.z);

        return Vector3(
                Math::RadiansToDegrees(euler.x),
                Math::RadiansToDegrees(euler.y),
                Math::RadiansToDegrees(euler.z)
            );
    }

    // calculate the difference in rotation between two angles along each axis.
    Vector3 CalculateAngularRotation(const Vector3& v1, const Vector3& v2)
    { return NormalizeAngles(Vector3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z));}

// Transform functions:

    // constructors and destructors
    Transform::Transform(Game &engine,
                         guid_t owner,
                         Transform* parentTransform,
                         Vector3 position,
                         Vector3 rotation,
                         Vector3 scale):
            Component(engine, owner),
            localPosition(position),
            localRotation(rotation),
            localScale(scale),
            modelMatrix(glm::identity<glm::mat4>()) { SetParent(parentTransform);}

    Transform::Transform(Transform &&other) noexcept:
            Component(other),
            localPosition(other.localPosition),
            localRotation(other.localRotation),
            localScale(other.localScale),
            modelMatrix(other.modelMatrix),
            children(std::move(other.children))
    {
        owner = other.owner;
        SetParent(other.m_parent);
    }

    void Transform::Invalidate() {
        if (m_parent != nullptr)
            m_parent->RemoveChild(this);
        if(!children.empty()){
            auto childrenCopy = children;
            for (auto child: childrenCopy)
                child->Invalidate();
        }
        children.clear();
        // remove all entities that aren't marked for deletion. Could result in endless loop!
        if(!entity()->isDeleted())
            engine.entityManager.deleteEntity(*entity());
        Component::Invalidate();
    }

    // getters
    glm::mat4 Transform::GetModelMatrix() {
        if(!m_parent)
            return modelMatrix;

        return m_parent->GetModelMatrix() * modelMatrix;
    }

    Vector3 Transform::GetGlobalPosition() {
        if(m_parent){
            auto p_model = m_parent->GetModelMatrix();
            glm::vec4 worldPos = p_model * glm::vec4((glm::vec3) localPosition, 1.0f);
            return Vector3(worldPos.x, worldPos.y, worldPos.z);
        }
        return localPosition;
    }
    Vector3 Transform::GetGlobalRotation() const{
        if (m_parent) {
            auto p_rotation = m_parent->GetGlobalRotation();
            glm::mat4 parentGlobalRotationMatrix = glm::eulerAngleYXZ(
                    glm::radians(p_rotation.y),
                    glm::radians(p_rotation.x),
                    glm::radians(p_rotation.z)
            );
            glm::mat4 localRotationMatrix = glm::eulerAngleYXZ(
                    glm::radians(localRotation.y),
                    glm::radians(localRotation.x),
                    glm::radians(localRotation.z)
            );
            glm::mat4 globalRotationMatrix = parentGlobalRotationMatrix * localRotationMatrix;

            return ConvertMatrixToEuler(globalRotationMatrix);
        } else
            return localRotation;
    }
    Vector3 Transform::GetGlobalScale() {
        if(m_parent){
            auto parentScale = m_parent->GetGlobalScale();
            return Vector3(
                    parentScale.x * localScale.x,
                    parentScale.y * localScale.y,
                    parentScale.z * localScale.z
            );
        }
        return localScale;
    }

    // setters
    void Transform::SetGlobalPosition(const Vector3& pos) {
        if (m_parent) {
            Vector3 parentGlobalPos = m_parent->GetGlobalPosition();
            localPosition = pos - parentGlobalPos;
        }
        else
            localPosition = pos;
        m_positionChanged = true;
        MarkDirty();
    }

    void Transform::SetGlobalRotation(const Vector3& newRotation) {
        Vector3 angularDiff;
        if (m_parent) {
            glm::mat4 parentGlobalRotationMatrix = glm::eulerAngleYXZ(
                    glm::radians(m_parent->GetGlobalRotation().y),
                    glm::radians(m_parent->GetGlobalRotation().x),
                    glm::radians(m_parent->GetGlobalRotation().z)
            );
            glm::mat4 inverseParentRotationMatrix = glm::inverse(parentGlobalRotationMatrix);
            glm::mat4 globalRotationMatrix = glm::eulerAngleYXZ(
                    glm::radians(newRotation.y),
                    glm::radians(newRotation.x),
                    glm::radians(newRotation.z)
            );
            glm::mat4 localRotationMatrix = inverseParentRotationMatrix * globalRotationMatrix;
            auto newEulerRotation = NormalizeAngles(ConvertMatrixToEuler(localRotationMatrix));
            angularDiff = CalculateAngularRotation(localRotation, newEulerRotation);
            localRotation = newRotation;
        } else {
            angularDiff = CalculateAngularRotation(localRotation, NormalizeAngles(newRotation));
            localRotation = NormalizeAngles(newRotation);
        }
        m_rotationChanged = true;
        MarkDirty();
    }
    void Transform::SetGlobalScale(const Vector3& scale) {
        auto oldScale = GetGlobalScale();
        if (m_parent) {
            Vector3 parentGlobalScale = m_parent->GetGlobalScale();
            localScale = Vector3(
                    scale.x / parentGlobalScale.x,
                    scale.y / parentGlobalScale.y,
                    scale.z / parentGlobalScale.z
            );
        }
        else
            localScale = scale;
        SyncScale(oldScale, localScale);
        MarkDirty();
    }
    void Transform::SetLocalPosition(const Vector3& pos) {
        localPosition = pos;
        m_positionChanged = true;
        MarkDirty();
    }
    void Transform::SetLocalRotation(const Vector3& rotation) {
        auto newRotation = NormalizeAngles(rotation);
        localRotation = newRotation;
        m_rotationChanged = true;
        MarkDirty();
    }
    void Transform::SetLocalScale(const Vector3& scale) {
        auto oldScale = GetGlobalScale();
        localScale = scale;
        SyncScale(oldScale, GetGlobalScale());
        MarkDirty();
    }

    // transformations
    void Transform::Translate(const Vector3 &direction) { SetLocalPosition(localPosition + direction);}
    void Transform::Rotate(const Vector3 &vector) {
        SetLocalRotation(NormalizeAngles(localRotation + vector));
    }
    void Transform::Rescale(const Vector3 &scalingFactors) {
        SetLocalScale(Vector3
        (localScale.x * scalingFactors.x,
         localScale.y * scalingFactors.y,
         localScale.z * scalingFactors.z));
    }

    // parent-child-relations
    void Transform::SetParent(Transform *transform) {
        // remove transform from old parent transform?!
        m_parent = transform;
        if (m_parent != nullptr)
            m_parent->AddChild(this);
    }
    void Transform::AddChild(Transform *transform) {children.insert(transform);}
    void Transform::RemoveChild(Transform *transform) { children.erase(transform);}

    glm::mat4 Transform::GetLocalMatrix() {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, (glm::vec3) GetLocalPosition());
        // roll
        model = glm::rotate(model, glm::radians(GetLocalRotation().z),
                            glm::vec3(0.0f, 0.0f, 1.0f));
        // pitch
        model = glm::rotate(model, glm::radians(GetLocalRotation().x),
                            glm::vec3(1.0f, 0.0f, 0.0f));
        // yaw
        model = glm::rotate(model, glm::radians(GetLocalRotation().y),
                            glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, (glm::vec3) GetLocalScale());
        return model;
    }

    // physics syncing
    void Transform::SyncPosition(const Vector3& newPosition) {
        SetGlobalPosition(newPosition);
        m_positionChanged = false;
    }
    void Transform::SyncRotation(const Vector3& newRotation) {
        SetGlobalRotation(newRotation);
        m_rotationChanged = false;
    }
    void Transform::SyncScale(const Vector3& oldScale, const Vector3& newScale){
        auto collider = entity()->GetComponent<BoxCollider2D>();
        if(!collider)
            return;
        collider->RescaleBounds(oldScale, newScale);
    }

    void Transform::MarkDirty() {
        dirty = true;
        if(children.empty())
            return;

        for(auto child:children)
            child->MarkDirty();
    }
}
