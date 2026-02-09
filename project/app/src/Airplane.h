#pragma once

#include <EisEngine.h>

namespace RTR {
    enum AnimMode{
        RAW = 0,
        UNIFORM = 1
    };

    class Airplane {
    public:
        explicit Airplane(Game& game);
        Entity* getPlane() {return public_plane.get();}
        Entity* getAnimPlane() {return plane.get();}

        // anims
        void Animate();
        void ResetAnim();

        void ToggleLooping() {looping = !looping;}
        void TogglePlay() { pause = !pause;}
        void ToggleRotation() { noRot = !noRot;}
        void SetAnimMode(AnimMode newMode) { mode = newMode;}

        bool IsLooping() const { return looping;}
        bool IsPaused() const { return pause;}
        bool IsRotationEnabled() const { return !noRot;}
        // void ToggleAxes();

        void SetAnimSpeed(const float& val) {targetAnimSpeed = val;}
    private:
        // init
        shared_ptr<Entity> public_plane = nullptr;
        shared_ptr<Entity> plane = nullptr;
        bool displayAxes = false;
        const std::string assetPath = "3d-objects/j22.fbx";
        Vector3 posOffset = Vector3::zero;
        const Vector3 hiddenRotOffset = Vector3(0, 270, 0);
        // std::array<std::shared_ptr<Entity>, 3> axisRings;

        // anim stuff:
        void UpdateKeyframes();
        float GetAnimSpeed();
        const float AnimPosScale = 2.0f;
        bool looping = true;
        bool pause = true;
        bool noRot = false;
        AnimMode mode = RAW;
        float animProgress = 0.0f;
        /// \n target uniform anim speed in units/s.
        float targetAnimSpeed = 3.0f;
        /// \n distance-independent speed for raw temporal anim.
        const float rawAnimSpeed = 0.5f;
        std::vector<std::pair<Vector3, Vector3>> keyFrames;
        std::pair<int, int> activeKeyframes;
        Vector3 currentDir = Vector3::zero;
        float distBetweenKeyframes = 0;

        Quaternion startRotation = Vector3::zero;
        Quaternion targetRotation = Vector3::zero;
    };
}
