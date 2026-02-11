
#include "engine/systems/Camera.h"
#include "engine/Game.h"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/quaternion.hpp>

namespace EisEngine::systems {
    inline Vector3 ConvertMatrixToEuler(const glm::mat4& mat) {
        glm::vec3 euler;
        glm::extractEulerAngleYXZ(mat, euler.y, euler.x, euler.z);

        return Vector3(
                Math::RadiansToDegrees(euler.x),
                Math::RadiansToDegrees(euler.y),
                Math::RadiansToDegrees(euler.z)
        );
    }

    Camera::Camera(EisEngine::Game &engine, const Vector2& screenDimensions, CameraMode cameraMode):
    System(engine),
    m_screenWidth((int) screenDimensions.x),
    m_screenHeight((int) screenDimensions.y),
    aspectRatio(screenDimensions.x / screenDimensions.y),
    nearClip(cameraMode == PERSPECTIVE ? 0.1f : -1),
    farClip(100)
    {
        entity = &engine.entityManager.createEntity("Camera");
        transform = entity->transform;
        engine.onUpdate.addListener([&] (Game &game){
            // save old known window dimensions.
            int oldWidth = m_screenWidth;
            int oldHeight = m_screenHeight;

            // get current window dimensions
            auto dimensions = game.context.GetWindowSize();
            m_screenWidth = (int) dimensions.x;
            m_screenHeight = (int) dimensions.y;

            // return if no changes
            if(oldWidth == m_screenWidth && oldHeight == m_screenHeight) return;

            // if changes update aspect ratio and m_zoom factor.
            UpdateAspectRatio();
        });
    }

    void Camera::SetCameraMode(const EisEngine::systems::CameraMode &newMode) {
        mode = newMode;

        nearClip = mode == PERSPECTIVE ? 0.1f : -1;
    }

    Vector3 Camera::viewDirection() const {
        return transform->Forward();
    }

    void Camera::LookAt(const EisEngine::Vector3 &pos) const {
        auto q = glm::quatLookAt((glm::vec3) (pos - transform->GetGlobalPosition()).normalized(),
                                 (glm::vec3) Vector3::up);
        auto res = Vector3(eulerAngles(q));
        transform->SetLocalRotation(res);
    }

    glm::mat4 Camera::GetVPMatrix() {
        auto view = CalculateViewMatrix();
        auto projection = GetProjectionMatrix();
        return projection * view;
    }

    glm::mat4 Camera::CalculateViewMatrix() const {
        // zoom pos offsets here!
        glm::vec3 cameraPos = (glm::vec3) transform->GetGlobalPosition();
        // (facing negative Z at rotation (0, 0, 0))
        glm::vec3 cameraDir = (glm::vec3) transform->Forward();
        glm::vec3 upDir = (glm::vec3) transform->Up();
        glm::vec3 viewVector = cameraPos + cameraDir;

        return glm::lookAt(cameraPos, viewVector, upDir);
    }

    glm::mat4 Camera::GetProjectionMatrix() const {
        if(aspectRatio == -1)
            return glm::identity<glm::mat4>();

        switch(mode){
            case PERSPECTIVE:
                return glm::perspective(Math::DegreesToRadians(fov), aspectRatio, nearClip, farClip);
            case ORTHO:
                return glm::ortho(-aspectRatio / m_zoom,
                                  aspectRatio / m_zoom,
                                  -1.0f / m_zoom,
                                  1.0f / m_zoom,
                                  nearClip,
                                  farClip);
        }
        return glm::mat4(1.0f);
    }
}
