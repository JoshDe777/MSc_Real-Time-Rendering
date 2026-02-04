#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "engine/ecs/System.h"
#include "engine/ecs/Entity.h"

namespace EisEngine::systems {
    using Transform = EisEngine::components::Transform;
    using System = EisEngine::ecs::System;

    enum CameraMode{
        PERSPECTIVE = 0,
        ORTHO = 1
    };

    /// \n The system taking care of the camera, framing the game's graphics to the screen's display.
    /// \n Currently exclusively orthographic projection in -z direction.
    class Camera : public System {
    public:
        /// \n Creates a new camera for the scene.
        /// @param screenDimensions - Vector2: the width and height of the camera FOV in pixels.
        explicit Camera(Game &engine, const Vector2& screenDimensions, CameraMode cameraMode = PERSPECTIVE);

        /// \n Calculates and returns the view-projection matrix.
        /// \n Projection matrix being the directions in which the camera is pointing,
        /// \n View matrix being the camera's position in world space.
        /// @return projection x view, a 4x4 matrix representing the camera's FOV from its position
        glm::mat4 GetVPMatrix();

        /// \n Zooms into the scene, restraining the FOV.
        /// @param value: the zoom factor applied to the camera.\n
        /// \zoom @b in: value > 0 \n=> camera will zoom IN on the scene, RESTRAINING the FOV.\n
        /// \n @b out: value &lt 0 \n=> camera will zoom OUT of the scene, WIDENING the FOV.
        void Zoom(float value){ m_zoom = clamp(m_zoom + value, minZoomVal, maxZoomVal);}

        /// \n Hard set of the camera's zoom value.
        /// @param value: the zoom factor the camera will take on with this call.
        /// \default @b value = 1.
        void SetZoom(float value = 1) { m_zoom = clamp(value, minZoomVal, maxZoomVal);}

        /// \n Returns the current zoom factor.
        [[nodiscard]] float GetZoomFactor() const { return m_zoom;}
        /// \n Returns screen width in pixels.
        [[nodiscard]] int GetWidth() const { return m_screenWidth;}
        /// \n Returns screen height in pixels.
        [[nodiscard]] int GetHeight() const { return m_screenHeight;}

        /// \n A pointer to the transform assigned to this object.
        Transform *transform;
        /// \n Calculates the projection matrix, representing the transformation from
        /// camera space into 2D screen coordinates.
        [[nodiscard]] glm::mat4 GetProjectionMatrix() const;
        /// \n Calculates the view matrix, representing the transformation required to
        /// translate world space transforms to camera space.
        [[nodiscard]] glm::mat4 CalculateViewMatrix() const;

        /// \n Sets a new FOV value for the active camera.
        void SetFOV(const float& val) {fov = val;}
        /// \n Switches camera mode to the requested style.
        void SetCameraMode(const CameraMode& newMode);
        /// \n [Debug function] outputs the direction towards which the camera is looking.
        Vector3 viewDirection() const;
        /// \n Rotates the camera to look at a given point.
        void LookAt(const Vector3& pos) const;
    private:
        /// \n A pointer to the entity assigned to this object.
        Entity *entity;

        /// \n The camera's aspect ratio, as width/height.
        float aspectRatio;
        /// \n The distance at which the camera starts to clip due to proximity to the camera.
        float nearClip;
        /// \n The distance at which the camera starts to clip due to distance from the camera.
        float farClip = 10;
        /// \n the screen width in pixels.
        int m_screenWidth;
        /// \n the screen height in pixels.
        int m_screenHeight;
        /// \n the zoom value of the current camera.
        float m_zoom = 1.0f;
        /// \n the furthest the camera can zoom out of the scene.
        float minZoomVal = 0.01f;
        /// \n the nearest the camera can zoom into the scene.
        float maxZoomVal = 5.0f;
        /// \n the perspective camera FOV value in degrees.
        float fov = 90;
        /// \n The camera mode
        CameraMode mode = PERSPECTIVE;

        /// \n updates the aspect ratio to width/height.
        void UpdateAspectRatio() { aspectRatio = (float) m_screenWidth / (float) m_screenHeight; }
    };
}
