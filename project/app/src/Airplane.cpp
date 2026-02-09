#include "Airplane.h"

namespace RTR {
    Airplane::Airplane(EisEngine::Game &game) {
        auto entity = ResourceManager::Load3DObject(game, assetPath);
        plane = static_cast<shared_ptr<Entity>>(entity);
        plane->transform->SetLocalRotation(rotationOffsets);
        for(auto child: plane->transform->getChildren()){
            DEBUG_LOG("Plane object name is " + plane->name() + " (expecting RootNode).")
        }

        // init keyframes as a tuple of <pos, fwdDir>
        keyFrames = {
            {Vector3::zero * AnimPosScale, Vector3(-1, 0, 0)},
            {Vector3(-10, 5, 0) * AnimPosScale, Vector3(-2.0f/3, 1.0f/3, 0)},
            {Vector3(-5, 10, -2) * AnimPosScale, Vector3(2.0f, 0, -1.0f/3)},
            {Vector3(0, 6, -5) * AnimPosScale, Vector3(0, -1, 0)},
            {Vector3(-5, 1, -5) * AnimPosScale, Vector3(-1, 0, 0)},
            {Vector3(-15, 5, 0) * AnimPosScale, Vector3(0, 1.0f/3, 2.0f/3)},
            {Vector3(5, 10, 10) * AnimPosScale, Vector3(4.0f/5, 1.0f/5, 0)},
            {Vector3(15, 15, 5) * AnimPosScale, Vector3(1.0f/6, 2.0f/6, -1.0f/6)},
            {Vector3(15, 20, 0) * AnimPosScale, Vector3(-1, 0, 0)},
            {Vector3(10, 10, 0) * AnimPosScale, Vector3(-1.0f/5, -4.0f/5, 0)}
        };
        activeKeyframes = std::pair<int, int>(-1, 0);
        UpdateKeyframes();

        game.onUpdate.addListener([&] (Game& game){
            Animate();
        });
    }

    void Airplane::UpdateKeyframes() {
        // signal anim end if attempting a new lap of the anim on non-looping sequence.
        if(!looping && activeKeyframes.second == 0){
            ResetAnim();
            pause = true;
        }

        animProgress = 0.0f;
        activeKeyframes.first = (int) Math::Mod(activeKeyframes.first + 1, (float) keyFrames.size());
        activeKeyframes.second = (int) Math::Mod(activeKeyframes.second + 1, (float) keyFrames.size());

        // calculate direction AB = B - A (linear interpolation),
        // with AB the position of the current & next keyframe respectively.
        // INTENTIONALLY not normalized so that A + p * AB = B with p = animProgress in [0, 1]
        currentDir = keyFrames[activeKeyframes.second].first - keyFrames[activeKeyframes.first].first;
        distBetweenKeyframes = currentDir.magnitude();

        // rotation interpolation.
        // store current rotation & calculate target rotation
        startRotation = Quaternion::FromEulerXYZ(plane->transform->GetLocalRotation());
        targetRotation = Quaternion::GetRotationToTarget(
                plane->transform->Forward(),
                keyFrames[activeKeyframes.second].second
            );
    }

    void Airplane::ResetAnim() {
        looping = true;
        animProgress = 1.0f;
        activeKeyframes = std::pair<int, int>(-1, 0);
        plane->transform->SetLocalPosition(Vector3::zero);
    }

    void Airplane::Animate() {
        if(pause)
            return;

        // only update keyframes after mapping to full position.
        if(animProgress == 1.0f)
            UpdateKeyframes();

        auto v = GetAnimSpeed();
        animProgress = Math::Clamp(animProgress + v * Time::deltaTime, 0.0f, 1.0f);
        plane->transform->SetLocalPosition(keyFrames[activeKeyframes.first].first + animProgress * currentDir);

        auto rot = Quaternion::Lerp(startRotation, targetRotation, animProgress);
        plane->transform->SetLocalRotation(rot.ToEulerXYZ());
    }

    float Airplane::GetAnimSpeed() {
        switch(mode){
            case RAW:
                return rawAnimSpeed;
            case UNIFORM:
                // since dist could technically be 0 = undefined behaviour, return no speed if no dist.
                return distBetweenKeyframes != 0 ? targetAnimSpeed / distBetweenKeyframes : 0.0f;
        }
    }
}
