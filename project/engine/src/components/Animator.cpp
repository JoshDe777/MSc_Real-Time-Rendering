#include "engine/components/Animator.h"
#include "engine/systems/Time.h"

using namespace EisEngine::systems;

namespace EisEngine::components {
    Animator::Animator(Game& engine, guid_t owner, std::vector<std::shared_ptr<AnimState>>& animStates) :
    Component(engine, owner), states(animStates) { }

    void Animator::Update(){
        auto activeState = states[currentState].get();
        if(newState){
            activeState->onEnter();
            elapsedTime = 0.0f;
            newState = false;
        }

        if(activeState->run(elapsedTime)){
            newState = true;
            currentState++;
        }
        elapsedTime += Time::deltaTime;
    }
}
