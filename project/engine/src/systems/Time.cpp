#pragma once

#include "engine/systems/Time.h"
#include "engine/Game.h"

namespace EisEngine::systems {
    Time::Time(Game &engine) : System(engine)
    { engine.onUpdate.addListener([&] (Game &engine){ UpdateDeltaTime();});}

    float Time::deltaTime = 1.0f/60.0f;

    void Time::UpdateDeltaTime() {
        auto frameTime = (float) glfwGetTime();
        deltaTime = frameTime - lastFrameTime;
        lastFrameTime = frameTime;
    }
}
