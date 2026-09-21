#include "Airplane.h"

namespace RTR {
    Airplane::Airplane(EisEngine::Game &game) {
        auto parent = &game.entityManager.createEntity("AirplaneParent");
        auto entity = ResourceManager::Load3DObject(game, assetPath);
        public_plane = static_cast<shared_ptr<Entity>>(parent);
        plane = static_cast<shared_ptr<Entity>>(entity);
        plane->transform->SetParent(parent->transform);
        for(auto child: plane->transform->getChildren()){
            child->SetLocalRotation(hiddenRotOffset);
        }

        // init keyframes as a tuple of <pos, fwdDir>
        keyFrames = {
            {Vector3::zero * AnimPosScale, Vector3::zero},
            {Vector3(-10, 5, 0) * AnimPosScale, Vector3(0, 0, -60)},
            {Vector3(-5, 10, -2) * AnimPosScale, Vector3(45, 0, 180)},
            {Vector3(0, 6, -5) * AnimPosScale, Vector3(0, 0, 270)},
            {Vector3(-5, 1, -5) * AnimPosScale, Vector3::zero},
            {Vector3(-15, 5, 0) * AnimPosScale, Vector3(0, 90, 45)},
            {Vector3(5, 10, 10) * AnimPosScale, Vector3(0, 180, 45)},
            {Vector3(15, 15, 5) * AnimPosScale, Vector3(-45, 270, 60)},
            {Vector3(15, 20, 0) * AnimPosScale, Vector3(0, 0, 0)},
            {Vector3(10, 10, 0) * AnimPosScale, Vector3(0, 0, -45)}
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
        startRotation = Quaternion::FromEulerXYZ(keyFrames[activeKeyframes.first].second);
        targetRotation = Quaternion::FromEulerXYZ(keyFrames[activeKeyframes.second].second);
    }

    void Airplane::ResetAnim() {
        looping = true;
        animProgress = 1.0f;
        activeKeyframes = std::pair<int, int>(-1, 0);
        plane->transform->SetLocalPosition(Vector3::zero);
        plane->transform->SetLocalRotation(Vector3::zero);
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

        if(noRot)
            return;

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
